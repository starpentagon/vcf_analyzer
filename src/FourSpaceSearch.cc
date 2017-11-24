#include <set>  // todo delete

#include "FourSpaceSearch.h"

using namespace std;

namespace realcore{

FourSpaceSearch::FourSpaceSearch(const BitBoard &bit_board)
: BitBoard(bit_board), attack_player_(kBlackTurn)
{
  vector<MovePosition> null_rest_list;
  vector<FourSpace> null_four_space_list;
  
  // kInvalidFourID用にダミーデータを設定する
  relaxed_four_list_.emplace_back(
    kNullMove, kNullMove, null_rest_list
  );
}

const RelaxedFourID FourSpaceSearch::AddRelaxedFour(const MovePosition gain_position, const MovePosition cost_position, const vector<MovePosition> &rest_list)
{
  RelaxedFour relaxed_four(gain_position, cost_position, rest_list);
  return AddRelaxedFour(relaxed_four);
}

const RelaxedFourID FourSpaceSearch::AddRelaxedFour(const RelaxedFour &relaxed_four)
{
  const auto key = relaxed_four.GetKey();
  const auto find_it = transposition_table_.find(key);

  if(find_it != transposition_table_.end()){
    return find_it->second;
  }

  relaxed_four_list_.emplace_back(relaxed_four);

  const RelaxedFourID four_id = relaxed_four_list_.size() - 1;
  
  const auto gain_position = relaxed_four.GetGainPosition();
  const auto cost_position = relaxed_four.GetCostPosition();
  auto rest_position_list = relaxed_four.GetRestPositionList();

  move_gain_list_[gain_position].emplace_back(four_id);
  move_cost_list_[cost_position].emplace_back(four_id);

  const auto rest_key = GetOpenRestKey(rest_position_list);

  for(const auto rest_position : rest_position_list){
    move_open_rest_list_[rest_position].emplace_back(four_id);
    move_rest_key_list_[rest_position].insert(rest_key);
  }

  transposition_table_.insert(make_pair(key, four_id));

  auto rest_find_it = rest_list_relaxed_four_list_.find(rest_key);

  if(rest_find_it == rest_list_relaxed_four_list_.end()){
    vector<RelaxedFourID> relaxed_four_list{four_id};
    rest_list_relaxed_four_list_.insert(make_pair(rest_key, relaxed_four_list));
  }else{
    rest_find_it->second.emplace_back(four_id);
  }

  return four_id;
}

size_t FourSpaceSearch::GetRestableRelaxedFourIDList(const MovePosition gain_position, const BoardDirection direction, std::vector<RelaxedFourID> * const restable_four_id_list) const
{
  assert(restable_four_id_list != nullptr);
  const BoardPosition gain_board_position = GetBoardPosition(gain_position, direction);
  size_t open_rest_count = 0;

  for(size_t i=1; i<5; i++){
    const auto check_position = gain_board_position + i;

    if(!IsInBoard(check_position)){
      break;
    }

    if(GetState(check_position) != kOpenPosition){
      continue;
    }

    const auto check_move = GetBoardMove(check_position);
    const auto &gain_four_id_list = move_gain_list_[check_move];

    if(!gain_four_id_list.empty()){
      restable_four_id_list->insert(restable_four_id_list->end(), gain_four_id_list.begin(), gain_four_id_list.end());
      open_rest_count++;
    }
  }

  for(size_t i=1; i<5; i++){
    const auto check_position = gain_board_position - i;

    if(!IsInBoard(check_position)){
      break;
    }

    if(GetState(check_position) != kOpenPosition){
      continue;
    }

    const auto check_move = GetBoardMove(check_position);
    const auto &gain_four_id_list = move_gain_list_[check_move];
    
    if(!gain_four_id_list.empty()){
      restable_four_id_list->insert(restable_four_id_list->end(), gain_four_id_list.begin(), gain_four_id_list.end());
      open_rest_count++;
    }
  }

  return open_rest_count;
}

const size_t FourSpaceSearch::GetMaxRelaxedFourLength() const
{
  size_t max_length = 0;

  for(const auto move : GetAllInBoardMove()){
    for(const auto& four_space : GetFourSpaceList(move)){
      const auto length = four_space.GetGainBit().count();
      max_length = max(length, max_length);
    }
  }

  return max_length;
}

void FourSpaceSearch::EnumerateLeaf(vector<RelaxedFourID> * const leaf_id_list) const
{
  // todo implement
  return;

  assert(leaf_id_list != nullptr);
  set<RelaxedFourID> dependent_node;

  for(size_t i=1, size=relaxed_four_list_.size(); i<size; i++){
    const auto &relaxed_four = GetRelaxedFour(i);
    const auto &rest_four_id_list = relaxed_four.GetRestPositionList();

    for(const auto rest_four_id : rest_four_id_list){
      dependent_node.insert(rest_four_id);
    }
  }

  for(size_t i=1, size=relaxed_four_list_.size(); i<size; i++){
    if(dependent_node.find(i) == dependent_node.end()){
      leaf_id_list->emplace_back(i);
    }
  }
}

const bool FourSpaceSearch::IsRegisteredFourSpace(const MovePosition move, const FourSpace &four_space) const
{
  const auto &four_space_list = GetFourSpaceList(move);

  for(const auto &registered_four_space : four_space_list){
    if(registered_four_space == four_space){
      return true;
    }
  }

  return false;
}

void FourSpaceSearch::EnumeratePuttableFourSpace(const MovePosition move, const FourSpace &four_space, const std::vector<MovePosition> &rest_list, std::vector<FourSpace> * const puttable_four_space_list)
{
  assert(puttable_four_space_list != nullptr);
  assert(puttable_four_space_list->empty());

  const auto rest_size = rest_list.size();
  assert(rest_size <= 2);

  auto dummy = rest_list; // todo modify
  const auto rest_list_key = GetOpenRestKey(dummy);
  vector<FourSpace> four_space_list{four_space};

  vector<MovePosition> all_rest_list{rest_list};
  all_rest_list.emplace_back(move);
  const auto all_rest_key = GetOpenRestKey(all_rest_list);
  const auto all_rest_size = rest_size + 1;

  if(rest_list_puttable_four_space_.find(all_rest_key) == rest_list_puttable_four_space_.end()){
    // 残路リストキーが初出の場合は全パターンを生成する
    if(all_rest_size == 1){
      puttable_four_space_list->emplace_back(four_space);
    }else if(all_rest_size == 2){
      const auto& rest_four_space_list_0 = GetFourSpaceList(all_rest_list[0]);
      const auto& rest_four_space_list_1 = GetFourSpaceList(all_rest_list[1]);

      GeneratePuttableFourSpace(rest_four_space_list_0, rest_four_space_list_1, puttable_four_space_list);
    }else{
      vector<FourSpace> puttable_four_space_list_pre;

      if(rest_list_puttable_four_space_.find(rest_list_key) == rest_list_puttable_four_space_.end()){
        const auto& rest_four_space_list_0 = GetFourSpaceList(rest_list[0]);
        const auto& rest_four_space_list_1 = GetFourSpaceList(rest_list[1]);
  
        GeneratePuttableFourSpace(rest_four_space_list_0, rest_four_space_list_1, &puttable_four_space_list_pre);
        AddRestListFourSpace(rest_list_key, puttable_four_space_list_pre);
      }else{
        puttable_four_space_list_pre = rest_list_puttable_four_space_[rest_list_key];
      }

      const auto& rest_four_space_list_2 = GetFourSpaceList(move);
      GeneratePuttableFourSpace(rest_four_space_list_2, puttable_four_space_list_pre, puttable_four_space_list);
    }

    AddRestListFourSpace(all_rest_key, *puttable_four_space_list);
    return;
  }

  if(rest_size == 0){
    puttable_four_space_list->emplace_back(four_space);
  }else if(rest_size == 1){
    const auto& rest_four_space_list_0 = GetFourSpaceList(rest_list[0]);
    GeneratePuttableFourSpace(four_space_list, rest_four_space_list_0, puttable_four_space_list);
    //cerr << "EnumeratePuttableFourSpace-1: " << rest_four_space_list_0.size() << " -> " << puttable_four_space_list->size() << endl;
  }else{
    vector<FourSpace> puttable_four_space_list_pre;

    if(rest_list_puttable_four_space_.find(rest_list_key) == rest_list_puttable_four_space_.end()){
      const auto& rest_four_space_list_0 = GetFourSpaceList(rest_list[0]);
      const auto& rest_four_space_list_1 = GetFourSpaceList(rest_list[1]);

      GeneratePuttableFourSpace(rest_four_space_list_0, rest_four_space_list_1, &puttable_four_space_list_pre);
      AddRestListFourSpace(rest_list_key, puttable_four_space_list_pre);
    }else{
      // todo delete
/*      vector<FourSpace> dummy, dummy_uniq;
      const auto& rest_four_space_list_0 = GetFourSpaceList(rest_list[0]);
      const auto& rest_four_space_list_1 = GetFourSpaceList(rest_list[1]);

      GeneratePuttableFourSpace(rest_four_space_list_0, rest_four_space_list_1, &dummy);

      for(const auto &elem : dummy){
        bool is_same = false;

        for(const auto elem2 : dummy_uniq){
          if(elem == elem2){
            is_same = true;
            break;
          }
        }

        if(!is_same){
          dummy_uniq.emplace_back(elem);
        }
      }

      const auto test_four_space_0 = rest_list_puttable_four_space_[rest_list[0]];
      const auto test_four_space_1 = rest_list_puttable_four_space_[rest_list[1]];
      const auto &test = rest_list_puttable_four_space_[rest_list_key];
      assert(dummy_uniq.size() == test.size());
*/
      puttable_four_space_list_pre = rest_list_puttable_four_space_[rest_list_key];
    }

    GeneratePuttableFourSpace(four_space_list, puttable_four_space_list_pre, puttable_four_space_list);
    //cerr << "EnumeratePuttableFourSpace2-1: " << four_space_list.size() * rest_four_space_list_0.size() << " -> " << puttable_four_space_list_pre.size() << endl;
  }

  AddRestListFourSpace(all_rest_key, *puttable_four_space_list);
}

void FourSpaceSearch::GeneratePuttableFourSpace(const vector<FourSpace> &four_space_list_1, const vector<FourSpace> &four_space_list_2, vector<FourSpace> * const puttable_four_space_list) const
{
  assert(puttable_four_space_list != nullptr);
  assert(puttable_four_space_list->empty());
  
  puttable_four_space_list->reserve(four_space_list_1.size() * four_space_list_2.size());

  for(const auto &four_space_1 : four_space_list_1){
    for(const auto &four_space_2 : four_space_list_2){
      if(!four_space_1.IsPuttable(four_space_2)){
        continue;
      }

      FourSpace four_space(four_space_1);
      four_space.Add(four_space_2);

      bool is_generated = false;

      for(const auto& generated_four_space : *puttable_four_space_list){
        if(generated_four_space == four_space){
          is_generated = true;
          break;
        }
      }

      if(is_generated){
        continue;
      }

      puttable_four_space_list->emplace_back(four_space);
    }
  }
}

const bool FourSpaceSearch::IsRegisteredLocalBitBoard(const MovePosition move, const LocalBitBoard &local_bitboard) const
{
  const auto& local_bitboard_list = move_local_bitboard_list_[move];

  for(const auto& registered_bitboard : local_bitboard_list){
    if(registered_bitboard == local_bitboard){
      return true;
    }
  }

  return false;
}

const RestListKey FourSpaceSearch::GetOpenRestKey(vector<MovePosition> &rest_list) const
{
  RestListKey rest_key = 0ULL;

  if(rest_list.empty()){
    return rest_key;
  }

  sort(rest_list.begin(), rest_list.end(), greater<MovePosition>());

  for(const auto rest_move : rest_list){
    rest_key = rest_key << 8;
    rest_key |= rest_move;
  }

  return rest_key;
}

void FourSpaceSearch::GetRestPosition(const MovePosition move, RestListKey rest_list_key, vector<MovePosition> * const rest_list) const
{
  assert(rest_list != nullptr);
  assert(rest_list->empty());
  assert(IsMoveInRestPosition(rest_list_key, move));
  static constexpr RestListKey kMoveMask = 0xFF;

  while(rest_list_key != 0){
    const auto rest_move = static_cast<MovePosition>(rest_list_key & kMoveMask);
    rest_list_key = rest_list_key >> 8;

    if(rest_move == move){
      continue;
    }

    rest_list->emplace_back(rest_move);
  }
}

void FourSpaceSearch::GetRestPosition(RestListKey rest_list_key, std::vector<MovePosition> * const rest_list) const
{
  assert(rest_list != nullptr);
  assert(rest_list->empty());
  static constexpr RestListKey kMoveMask = 0xFF;

  while(rest_list_key != 0){
    const auto rest_move = static_cast<MovePosition>(rest_list_key & kMoveMask);
    rest_list_key = rest_list_key >> 8;

    rest_list->emplace_back(rest_move);
  }
}

const bool FourSpaceSearch::IsRegisteredFourSpace(const RestListKey rest_key, const FourSpace &four_space) const
{
  const auto find_it = rest_list_puttable_four_space_.find(rest_key);

  if(find_it == rest_list_puttable_four_space_.end()){
    return false;
  }

  const auto& four_space_list = find_it->second;

  for(const auto &registered_four_space : four_space_list){
    if(registered_four_space == four_space){
      return true;
    }
  }

  return false;
}

void FourSpaceSearch::ShowBoardRelaxedFourCount() const
{
  for(const auto move : GetAllInBoardMove()){
    Cordinate x, y;
    GetMoveCordinate(move, &x, &y);

    cerr << move_gain_list_[move].size() << ",";

    if(x == 15){
      cerr << endl;
    }
  }
}

void FourSpaceSearch::ShowBoardGainCostSpaceCount() const
{
  for(const auto move : GetAllInBoardMove()){
    Cordinate x, y;
    size_t four_space_count = 0;
    GetMoveCordinate(move, &x, &y);

    four_space_count = GetFourSpaceList(move).size();

    cerr << four_space_count << ",";

    if(x == 15){
      cerr << endl;
    }
  }  
}

const bool FourSpaceSearch::IsMoveInRestPosition(const RestListKey rest_list_key, const MovePosition move) const
{
  const bool is_move_in_rest = ((move | move << 8 | move << 16) & rest_list_key) != 0;
  return is_move_in_rest;
}

const bool FourSpaceSearch::IsProperSubset(const RestListKey sub_rest_key, const RestListKey super_rest_key, MovePosition * const additional_move) const
{
  assert(additional_move != nullptr);

  if(sub_rest_key == super_rest_key){
    return false;
  }

  vector<MovePosition> sub_rest, super_rest;
  GetRestPosition(sub_rest_key, &sub_rest);
  GetRestPosition(super_rest_key, &super_rest);

  MoveBitSet super_bit;

  for(const auto move : super_rest){
    super_bit.set(move);
  }

  for(const auto move : sub_rest){
    if(!super_bit[move]){
      return false;
    }

    super_bit.reset(move);
  }

  MoveList additional_move_list;
  GetMoveList(super_bit, &additional_move_list);
  assert(additional_move_list.size() == 1);

  *additional_move = additional_move_list[0];

  return true;
}

const size_t FourSpaceSearch::GetRestListSize(const RestListKey rest_key) const
{
  static constexpr size_t kMoveMask = 255;
  size_t count = (rest_key & kMoveMask) != 0 ? 1 : 0;
  count += (rest_key & (kMoveMask << 8)) != 0 ? 1 : 0;
  count += (rest_key & (kMoveMask << 16)) != 0 ? 1 : 0;

  return count;
}

void FourSpaceSearch::UpdateRestListPuttableFourSpace(const RestListKey rest_key, const FourSpace &four_space, std::map<RestListKey, std::vector<FourSpace>> * const additional_four_space)
{
  assert(additional_four_space != nullptr);

  if(IsRegisteredFourSpace(rest_key, four_space)){
    return;
  }

  AddRestListFourSpace(rest_key, four_space);
  (*additional_four_space)[rest_key].emplace_back(four_space);

  const auto find_it = rest_key_tree_.find(rest_key);

  if(find_it == rest_key_tree_.end()){
    return;
  }

  const auto& child_rest_key_set = find_it->second;

  std::vector<FourSpace> four_space_list{four_space};

  for(const auto child_rest_key : child_rest_key_set){
    MovePosition additional_move;

    if(!IsProperSubset(rest_key, child_rest_key, &additional_move)){
      continue;
    }
  
    std::vector<FourSpace> next_four_space_list;
    const auto& registered_four_space_list = GetFourSpaceList(additional_move);
    GeneratePuttableFourSpace(four_space_list, registered_four_space_list, &next_four_space_list);

    for(const auto next_four_space : next_four_space_list){
      UpdateRestListPuttableFourSpace(child_rest_key, next_four_space, additional_four_space);
    }
  }
}

const RestListKey FourSpaceSearch::GetRestList(const NextRelaxedFourInfo &next_four_info, std::vector<MovePosition> * const rest_list) const
{
  assert(rest_list != nullptr);

  const MovePosition rest_1 = std::get<2>(next_four_info);
  const MovePosition rest_2 = std::get<3>(next_four_info);
  const MovePosition rest_3 = std::get<4>(next_four_info);

  rest_list->reserve(3);

  for(const auto rest_move : {rest_1, rest_2, rest_3}){
    if(!move_gain_list_[rest_move].empty()){
      rest_list->emplace_back(rest_move);
    }
  }

  const auto rest_key = GetOpenRestKey(*rest_list);
  return rest_key;
}

void FourSpaceSearch::AddRestListFourSpace(const RestListKey rest_key, const FourSpace &four_space)
{
  // 五が生じる獲得/損失空間は生成しない
  // todo kBlackStone -> S, kWhiteStone -> T
  SetMoveBit<kBlackStone>(four_space.GetGainBit());
  SetMoveBit<kWhiteStone>(four_space.GetCostBit());

  const bool is_five = IsFiveStones<kBlackTurn>() || IsFiveStones<kWhiteTurn>();

  SetMoveBit<kOpenPosition>(four_space.GetGainBit());
  SetMoveBit<kOpenPosition>(four_space.GetCostBit());

  if(is_five){
    return;
  }

  if(IsRegisteredFourSpace(rest_key, four_space)){
    return;
  }

  rest_list_puttable_four_space_[rest_key].emplace_back(four_space);

  vector<MovePosition> rest_list;
  GetRestPosition(rest_key, &rest_list);

  for(const auto rest_move : rest_list){
    move_rest_key_list_[rest_move].insert(rest_key);
  }

  const auto rest_size = rest_list.size();
  assert(rest_size >= 1 && rest_size <= 3);

  if(rest_size == 2){
    for(const auto rest_move : rest_list){
      rest_key_tree_[rest_move].insert(rest_key);
    }
  }else{
    static constexpr array<size_t, 3> index_min_list{{0, 0, 1}};
    static constexpr array<size_t, 3> index_max_list{{1, 2, 2}};
    
    for(size_t i=0; i<3; i++){
      const auto index_min = index_min_list[i];
      const auto index_max = index_max_list[i];

      const auto rest_min = rest_list[index_min];
      const auto rest_max = rest_list[index_max];

      vector<MovePosition> parent_rest_list{
        rest_min, rest_max
      };

      const auto parent_rest_key = GetOpenRestKey(parent_rest_list);
      rest_key_tree_[parent_rest_key].insert(rest_key);
      rest_key_tree_[index_min].insert(parent_rest_key);
      rest_key_tree_[index_max].insert(parent_rest_key);
    }
  }
}

void FourSpaceSearch::AddRestListFourSpace(const RestListKey rest_key, const vector<FourSpace> &four_space_list)
{
  for(const auto& four_space : four_space_list){
    AddRestListFourSpace(rest_key, four_space);
  }
}

}   // namespace realcore
