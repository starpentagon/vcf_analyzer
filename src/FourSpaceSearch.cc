#include <set>  // todo delete

#include "MoveTree.h"   // todo delete
#include "HashTable.h"   // todo delete
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

void FourSpaceSearch::GetReachIDSequence(const RelaxedFourID relaxed_four_id, vector<RelaxedFourID> * const id_list) const
{
  // todo implement
  return;

  set<RelaxedFourID> id_set;
  GetReachIDSequence(relaxed_four_id, &id_set, id_list);
}

void FourSpaceSearch::GetReachIDSequence(const RelaxedFourID relaxed_four_id, set<RelaxedFourID> * const appeared_id_set, vector<RelaxedFourID> * const id_list) const
{
  // todo implement
  return;

  if(relaxed_four_id == kInvalidFourID){
    return;
  }

  assert(appeared_id_set != nullptr);
  assert(id_list != nullptr);

  const auto find_it = appeared_id_set->find(relaxed_four_id);

  if(find_it != appeared_id_set->end()){
    return;
  }

  const RelaxedFour &relaxed_four = GetRelaxedFour(relaxed_four_id);
  const auto &rest_four_id_list = relaxed_four.GetRestPositionList();

  for(const auto rest_four_id : rest_four_id_list){
    vector<RelaxedFourID> rest_id_list;
    GetReachIDSequence(rest_four_id, appeared_id_set, &rest_id_list);

    id_list->insert(id_list->end(), rest_id_list.begin(), rest_id_list.end());
  }

  id_list->emplace_back(relaxed_four_id);
  appeared_id_set->insert(relaxed_four_id);
}

bool FourSpaceSearch::GetReachSequence(const RelaxedFourID relaxed_four_id, MoveList * const move_list) const
{
  // todo implement
  return false;

  vector<RelaxedFourID> four_id_list;
  GetReachIDSequence(relaxed_four_id, &four_id_list);
  return GetReachSequence(four_id_list, move_list);
}

bool FourSpaceSearch::GetReachSequence(const vector<RelaxedFourID> &four_id_list, MoveList * const move_list) const{
  // todo implement
  return false;

  assert(move_list != nullptr && move_list->empty());

  array<RelaxedFourID, kMoveNum> put_four_id{{kInvalidFourID}};

  for(const auto four_id : four_id_list){
    const auto &relaxed_four = GetRelaxedFour(four_id);
    
    const auto gain = relaxed_four.GetGainPosition();
    const auto cost = relaxed_four.GetCostPosition();

    if(put_four_id[gain] != kInvalidFourID && put_four_id[gain] != four_id){
      return false;
    }

    if(put_four_id[cost] != kInvalidFourID && put_four_id[cost] != four_id){
      return false;
    }

    put_four_id[gain] = four_id;
    put_four_id[cost] = four_id;

    *move_list += gain;
    *move_list += cost;
  }

  return true;
}

const size_t FourSpaceSearch::GetMaxRelaxedFourLength() const
{
  // todo implement
  return 0;

  size_t max_length = 0;

  for(size_t i=1, size=relaxed_four_list_.size(); i<size; i++){
    vector<RelaxedFourID> four_id_list;
    GetReachIDSequence(i, &four_id_list);
    const size_t length = four_id_list.size();

    max_length = max(max_length, length);
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

const bool FourSpaceSearch::IsExpanded(const RelaxedFourID relaxed_four_id) const
{
  const auto &relaxed_four = GetRelaxedFour(relaxed_four_id);
  const auto gain_position = relaxed_four.GetGainPosition();

  const auto &gain_four_id_list = move_gain_list_[gain_position];
  const auto find_it = find(gain_four_id_list.begin(), gain_four_id_list.end(), relaxed_four_id);

  return find_it != gain_four_id_list.end();
}

void FourSpaceSearch::GetDependentReachMove(const RelaxedFourID relaxed_four_id, vector<vector<MovePair>> * const move_pair_vector) const
{
  assert(move_pair_vector != nullptr);

  const auto &relaxed_four = GetRelaxedFour(relaxed_four_id);
  const auto &rest_move_list = relaxed_four.GetRestPositionList();
  MoveList cost_move_list[3];
  const auto size = rest_move_list.size();

  for(size_t i=0; i<size; i++){
    const auto rest_move = rest_move_list[i];
    GetCostMoveList(rest_move, &(cost_move_list[i]));
  }

  MoveBitSet conflict_bit;

  const auto gain_move = relaxed_four.GetGainPosition();
  const auto cost_move = relaxed_four.GetCostPosition();
  MovePair four_pair(gain_move, cost_move);
  
  conflict_bit.set(gain_move);
  conflict_bit.set(cost_move);

  if(size == 0){
    const vector<MovePair> move_pair_list{four_pair};
    move_pair_vector->emplace_back(move_pair_list);
  }else if(size == 1){
    const auto rest_move_0 = rest_move_list[0];

    if(!conflict_bit[rest_move_0]){
      for(const auto rest_cost_move_0 : cost_move_list[0]){
        if(conflict_bit[rest_cost_move_0]){
          continue;
        }

        MovePair rest_move_pair_0(rest_move_0, rest_cost_move_0);
        const vector<MovePair> move_pair_list{rest_move_pair_0, four_pair};

        move_pair_vector->emplace_back(move_pair_list);
      }
    }
  }else if(size == 2){
    const auto rest_move_0 = rest_move_list[0];

    if(!conflict_bit[rest_move_0]){
      for(const auto rest_cost_move_0 : cost_move_list[0]){
        if(conflict_bit[rest_cost_move_0]){
          continue;
        }

        conflict_bit.set(rest_move_0);
        conflict_bit.set(rest_cost_move_0);
        
        const auto rest_move_1 = rest_move_list[1];

        if(!conflict_bit[rest_move_1]){
          for(const auto rest_cost_move_1 : cost_move_list[1]){
            if(conflict_bit[rest_cost_move_1]){
              continue;
            }

            MovePair rest_move_pair_0(rest_move_0, rest_cost_move_0);
            MovePair rest_move_pair_1(rest_move_1, rest_cost_move_1);
            const vector<MovePair> move_pair_list{rest_move_pair_0, rest_move_pair_1, four_pair};
    
            move_pair_vector->emplace_back(move_pair_list);
          }
        }
    
        conflict_bit.reset(rest_move_0);
        conflict_bit.reset(rest_cost_move_0);
      }
    }
  }else if(size == 3){
    const auto rest_move_0 = rest_move_list[0];
    
    if(!conflict_bit[rest_move_0]){
      for(const auto rest_cost_move_0 : cost_move_list[0]){
        if(conflict_bit[rest_cost_move_0]){
          continue;
        }

        conflict_bit.set(rest_move_0);
        conflict_bit.set(rest_cost_move_0);
        
        const auto rest_move_1 = rest_move_list[1];

        if(!conflict_bit[rest_move_1]){
          for(const auto rest_cost_move_1 : cost_move_list[1]){
            if(conflict_bit[rest_cost_move_1]){
              continue;
            }

            conflict_bit.set(rest_move_1);
            conflict_bit.set(rest_cost_move_1);
            
            const auto rest_move_2 = rest_move_list[2];
    
            if(!conflict_bit[rest_move_2]){
              for(const auto rest_cost_move_2 : cost_move_list[2]){
                if(conflict_bit[rest_cost_move_2]){
                  continue;
                }
    

                MovePair rest_move_pair_0(rest_move_0, rest_cost_move_0);
                MovePair rest_move_pair_1(rest_move_1, rest_cost_move_1);
                MovePair rest_move_pair_2(rest_move_2, rest_cost_move_2);
                const vector<MovePair> move_pair_list{rest_move_pair_0, rest_move_pair_1, rest_move_pair_2, four_pair};
        
                move_pair_vector->emplace_back(move_pair_list);
              }
            }

            conflict_bit.reset(rest_move_1);
            conflict_bit.reset(rest_cost_move_1);
          }
        }
    
        conflict_bit.reset(rest_move_0);
        conflict_bit.reset(rest_cost_move_0);
      }
    }
  }
}

void FourSpaceSearch::GetCostMoveList(const MovePosition gain_move, MoveList * const cost_move_list) const
{
  assert(cost_move_list != nullptr);

  const auto &gain_four_id_list = move_gain_list_[gain_move];
  MoveBitSet cost_move_bit;

  for(const auto relaxed_four_id : gain_four_id_list){
    const auto &relaxed_four = GetRelaxedFour(relaxed_four_id);
    const MovePosition cost_move = relaxed_four.GetCostPosition();

    if(cost_move_bit[cost_move]){
      continue;
    }

    cost_move_bit.set(cost_move);
    *cost_move_list += cost_move;
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

void FourSpaceSearch::EnumerateFourSpace(const MovePosition gain_position, const MovePosition cost_position, const std::vector<MovePosition> &rest_list, std::vector<FourSpace> * const four_space_list) const
{
  assert(four_space_list != nullptr);
  assert(four_space_list->empty());

  // todo delete
  static size_t count = 0;

  if(++count % 1000 == 0)
    cerr << "EnumerateFourSpace New: " << count << endl;
  
  const auto rest_size = rest_list.size();
  assert(rest_size <= 3);

  if(rest_size == 0){
    four_space_list->emplace_back(gain_position, cost_position);
    return;
  }

  auto dummy = rest_list;   // todo modify
  const auto rest_key = GetOpenRestKey(dummy);

  const auto find_it = rest_list_puttable_four_space_.find(rest_key);

  if(find_it == rest_list_puttable_four_space_.end()){
    return;
  }
  
  const auto& puttable_four_space_list = find_it->second;
  GenerateNonConflictFourSpace(gain_position, cost_position, puttable_four_space_list, four_space_list);
}

void FourSpaceSearch::EnumeratePuttableFourSpace(const FourSpace &four_space, const std::vector<MovePosition> &rest_list, std::vector<FourSpace> * const puttable_four_space_list)
{
  assert(puttable_four_space_list != nullptr);
  assert(puttable_four_space_list->empty());

  const auto rest_size = rest_list.size();
  assert(rest_size <= 2);

  if(rest_size == 0){
    puttable_four_space_list->emplace_back(four_space);
    return;
  }

  vector<FourSpace> four_space_list{four_space};

  if(rest_size == 1){
    const auto& rest_four_space_list_0 = GetFourSpaceList(rest_list[0]);

    GeneratePuttableFourSpace(four_space_list, rest_four_space_list_0, puttable_four_space_list);
    //cerr << "EnumeratePuttableFourSpace-1: " << rest_four_space_list_0.size() << " -> " << puttable_four_space_list->size() << endl;
  }else{
    auto dummy = rest_list; // todo modify
    const auto rest_list_key = GetOpenRestKey(dummy);

    vector<FourSpace> puttable_four_space_list_pre;

    if(rest_list_puttable_four_space_.find(rest_list_key) == rest_list_puttable_four_space_.end()){
      const auto& rest_four_space_list_0 = GetFourSpaceList(rest_list[0]);
      const auto& rest_four_space_list_1 = GetFourSpaceList(rest_list[1]);

      GeneratePuttableFourSpace(rest_four_space_list_0, rest_four_space_list_1, &puttable_four_space_list_pre);
      rest_list_puttable_four_space_[rest_list_key] = puttable_four_space_list_pre;
      move_rest_key_list_[rest_list[0]].insert(rest_list_key);
      move_rest_key_list_[rest_list[1]].insert(rest_list_key);
    }else{
      vector<FourSpace> dummy;
      const auto& rest_four_space_list_0 = GetFourSpaceList(rest_list[0]);
      const auto& rest_four_space_list_1 = GetFourSpaceList(rest_list[1]);

      GeneratePuttableFourSpace(rest_four_space_list_0, rest_four_space_list_1, &dummy);
      const auto &test = rest_list_puttable_four_space_[rest_list_key];
      assert(dummy.size() == test.size());

      puttable_four_space_list_pre = rest_list_puttable_four_space_[rest_list_key];
    }

    GeneratePuttableFourSpace(four_space_list, puttable_four_space_list_pre, puttable_four_space_list);
    //cerr << "EnumeratePuttableFourSpace2-1: " << four_space_list.size() * rest_four_space_list_0.size() << " -> " << puttable_four_space_list_pre.size() << endl;
  }
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

      puttable_four_space_list->emplace_back(four_space_1);
      puttable_four_space_list->back().Add(four_space_2);
    }
  }
}

void FourSpaceSearch::GenerateNonConflictFourSpace(const MovePosition gain_position, const MovePosition cost_position, const vector<FourSpace> &four_space_list, vector<FourSpace> * const non_conflict_four_space_list) const
{
  assert(non_conflict_four_space_list != nullptr);
  assert(non_conflict_four_space_list->empty());
  non_conflict_four_space_list->reserve(four_space_list.size());

  for(const auto &four_space : four_space_list){
    if(four_space.IsConflict(gain_position, cost_position)){
      continue;
    }

    non_conflict_four_space_list->emplace_back(four_space);
    non_conflict_four_space_list->back().Add(gain_position, cost_position);
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
  assert(((move | move << 8 | move << 16) & rest_list_key) != 0);
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

void FourSpaceSearch::EnumerateRestKeyFourSpaceList(const RestListKey rest_key, std::vector<FourSpace> * const puttable_four_space_list)
{
  assert(puttable_four_space_list != nullptr);

  const auto find_it = rest_list_puttable_four_space_.find(rest_key);

  if(find_it != rest_list_puttable_four_space_.end()){
    *puttable_four_space_list = find_it->second;
    return;
  }
}

}   // namespace realcore
