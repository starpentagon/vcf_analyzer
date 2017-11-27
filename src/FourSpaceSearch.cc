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
    make_unique<RelaxedFour>(kNullMove, kNullMove, null_rest_list)
  );
}

const pair<RelaxedFourID, bool> FourSpaceSearch::AddRelaxedFour(const NextRelaxedFourInfo &next_relaxed_four)
{
  const MovePosition next_gain = get<0>(next_relaxed_four);
  const MovePosition next_cost = get<1>(next_relaxed_four);

  vector<MovePosition> rest_list;
  GetRestList(next_relaxed_four, &rest_list);

  RelaxedFour relaxed_four(next_gain, next_cost, rest_list);
  return AddRelaxedFour(relaxed_four);
}

const pair<RelaxedFourID, bool> FourSpaceSearch::AddRelaxedFour(const RelaxedFour &relaxed_four)
{
  const auto key = relaxed_four.GetKey();
  const auto find_it = transposition_table_.find(key);

  if(find_it != transposition_table_.end()){
    return make_pair(find_it->second, false);
  }

  relaxed_four_list_.emplace_back(make_unique<RelaxedFour>(relaxed_four));

  const RelaxedFourID four_id = relaxed_four_list_.size() - 1;
  transposition_table_.insert(make_pair(key, four_id));
  
  auto rest_position_list = relaxed_four.GetRestPositionList();
  const auto rest_key = GetOpenRestKey(rest_position_list);

  const auto& four_id_find_it = rest_list_relaxed_four_list_.find(rest_key);

  if(four_id_find_it == rest_list_relaxed_four_list_.end()){
    const auto& insert_result =
    rest_list_relaxed_four_list_.insert(make_pair(rest_key, make_unique<vector<RelaxedFourID>>()));
    assert(insert_result.second);

    auto& relaxed_fourid_vector_ptr = insert_result.first->second;
    relaxed_fourid_vector_ptr->emplace_back(four_id);
  }else{
    auto& relaxed_fourid_vector_ptr = four_id_find_it->second;
    relaxed_fourid_vector_ptr->emplace_back(four_id);
  }

  return make_pair(four_id, true);
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
    const auto find_it = move_feasible_relaxed_four_id_list_.find(check_move);

    if(find_it == move_feasible_relaxed_four_id_list_.end()){
      continue;
    }

    // todo delete --
    if(check_move == kMoveEL){
      int a = 1;
    }

    assert(rest_list_puttable_four_space_.find(check_move) != rest_list_puttable_four_space_.end());
    // -- todo delete

    const auto& relaxed_id_set_ptr = find_it->second;

    for(const auto relaxed_four_id : *relaxed_id_set_ptr){
      restable_four_id_list->emplace_back(relaxed_four_id);
    }

    open_rest_count++;
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
    const auto find_it = move_feasible_relaxed_four_id_list_.find(check_move);

    if(find_it == move_feasible_relaxed_four_id_list_.end()){
      continue;
    }

    // todo delete --
    if(check_move == kMoveEL){
      int a = 1;
    }

    assert(rest_list_puttable_four_space_.find(check_move) != rest_list_puttable_four_space_.end());
    // -- todo delete

    const auto& relaxed_id_set_ptr = find_it->second;

    for(const auto relaxed_four_id : *relaxed_id_set_ptr){
      restable_four_id_list->emplace_back(relaxed_four_id);
    }

    open_rest_count++;
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

  const auto& four_space_list_ptr = find_it->second;

  for(const auto &registered_four_space : *four_space_list_ptr){
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

    const auto find_it = move_feasible_relaxed_four_id_list_.find(move);
    size_t count = 0;
    
    if(find_it != move_feasible_relaxed_four_id_list_.end()){
      const auto& relaxed_fourid_set_ptr = find_it->second;
      count = relaxed_fourid_set_ptr->size();
    }

    cerr << count << ",";

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

const MovePosition FourSpaceSearch::GetAdditionalMove(const RestListKey sub_rest_key, const RestListKey super_rest_key) const{
  vector<MovePosition> sub_rest, super_rest;
  GetRestPosition(sub_rest_key, &sub_rest);
  GetRestPosition(super_rest_key, &super_rest);

  MoveBitSet super_bit;
  
  for(const auto move : super_rest){
    super_bit.set(move);
  }

  for(const auto move : sub_rest){
    super_bit.reset(move);
  }
  
  MoveList additional_move_list;
  GetMoveList(super_bit, &additional_move_list);
  assert(additional_move_list.size() == 1);

  return additional_move_list[0];
}

const size_t FourSpaceSearch::GetRestListSize(const RestListKey rest_key) const
{
  static constexpr size_t kMoveMask = 0xFF;
  size_t count = (rest_key & kMoveMask) != 0 ? 1 : 0;
  count += (rest_key & (kMoveMask << 8)) != 0 ? 1 : 0;
  count += (rest_key & (kMoveMask << 16)) != 0 ? 1 : 0;

  return count;
}

const RestListKey FourSpaceSearch::GetRestList(const NextRelaxedFourInfo &next_four_info, std::vector<MovePosition> * const rest_list) const
{
  assert(rest_list != nullptr);
  assert(rest_list->empty());

  const MovePosition rest_1 = std::get<2>(next_four_info);
  const MovePosition rest_2 = std::get<3>(next_four_info);
  const MovePosition rest_3 = std::get<4>(next_four_info);

  rest_list->reserve(3);

  for(const auto rest_move : {rest_1, rest_2, rest_3}){
    const bool is_feasible = move_feasible_relaxed_four_id_list_.find(rest_move) != move_feasible_relaxed_four_id_list_.end();

    if(is_feasible){
      rest_list->emplace_back(rest_move);

      // todo delete --
      if(rest_move == kMoveEL && rest_list_puttable_four_space_.find(rest_move) == rest_list_puttable_four_space_.end()){
        int a = 1;
      }
      // -- todo delete
    }
  }

  const auto rest_key = GetOpenRestKey(*rest_list);
  return rest_key;
}

void FourSpaceSearch::GetGainMoveRelaxedFourIDList(MoveRelaxedFourIDList * const move_relaxed_four_id_list) const
{
  assert(move_relaxed_four_id_list != nullptr);
  
  for(const auto move : GetAllInBoardMove()){
    const auto find_it = move_feasible_relaxed_four_id_list_.find(move);

    if(find_it == move_feasible_relaxed_four_id_list_.end()){
      continue;
    }

    const auto& relaxed_four_id_set_ptr = find_it->second;

    for(const auto relaxed_four_id : *relaxed_four_id_set_ptr){
      (*move_relaxed_four_id_list)[move].emplace_back(relaxed_four_id);
    }
  }
}

void FourSpaceSearch::GetCostMoveRelaxedFourIDList(MoveRelaxedFourIDList * const move_relaxed_four_id_list) const
{
  assert(move_relaxed_four_id_list != nullptr);
  
  for(const auto move : GetAllInBoardMove()){
    const auto find_it = move_feasible_relaxed_four_id_list_.find(move);

    if(find_it == move_feasible_relaxed_four_id_list_.end()){
      continue;
    }

    const auto& relaxed_four_id_set_ptr = find_it->second;

    for(const auto relaxed_four_id : *relaxed_four_id_set_ptr){
      const auto& relaxed_four = GetRelaxedFour(relaxed_four_id);
      const auto cost = relaxed_four.GetCostPosition();

      (*move_relaxed_four_id_list)[cost].emplace_back(relaxed_four_id);
    }
  }
}

}   // namespace realcore
