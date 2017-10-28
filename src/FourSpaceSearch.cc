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
  const auto find_it = relaxed_four_transposition_map_.find(key);

  if(find_it != relaxed_four_transposition_map_.end()){
    // 登録済みの場合
    return find_it->second;
  }

  relaxed_four_list_.emplace_back(relaxed_four);

  const RelaxedFourID four_id = relaxed_four_list_.size() - 1;
  relaxed_four_transposition_map_.insert(pair<uint64_t, RelaxedFourID>(key, four_id));

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
    const auto &reach_id_list = reach_region_[check_move];

    if(!reach_id_list.empty()){
      restable_four_id_list->insert(restable_four_id_list->end(), reach_id_list.begin(), reach_id_list.end());
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
    const auto &reach_id_list = reach_region_[check_move];

    if(!reach_id_list.empty()){
      restable_four_id_list->insert(restable_four_id_list->end(), reach_id_list.begin(), reach_id_list.end());
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

  const RelaxedFour &relaxed_four = relaxed_four_list_[relaxed_four_id];
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
    const auto relaxed_four = relaxed_four_list_[four_id];
    
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

  // todo delete
  vector<RelaxedFourID> leaf_id_list;
  EnumerateLeaf(&leaf_id_list);

  for(const auto leaf_id : leaf_id_list){
    vector<RelaxedFourID> four_id_list;
    GetReachIDSequence(leaf_id, &four_id_list);
    
    const size_t length = four_id_list.size();
    
    if(length >= 80){
      MoveList debug;
      GetReachSequence(leaf_id, &debug);

      MoveList board("hhgigjfgjjfjeeifhkijlijhllefcjejhmdgmcdjmkjdbblkbefmblgmdnimibjcnbjmaemhaibgambnbocncahbcojndokbeankganljoabkaacmaadoafoochaoeonokoo");
      MoveTree proof;
      proof.AddChild(debug);

      stringstream ss;

      ss << "(;GM[4]FF[4]SZ[15]";
      ss << board.GetSGFPositionText();
      ss << (board.IsBlackTurn() ? "PL[B]" : "PL[W]");
      ss << proof.GetSGFLabeledText(board.IsBlackTurn());
      ss << ")";

      cerr << ss.str() << endl << endl;
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
    const auto relaxed_four = relaxed_four_list_[i];
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
  const auto &relaxed_four = relaxed_four_list_[relaxed_four_id];
  const auto gain_position = relaxed_four.GetGainPosition();

  const auto &reachable_four_list = reach_region_[gain_position];
  const auto find_it = find(reachable_four_list.begin(), reachable_four_list.end(), relaxed_four_id);

  return find_it != reachable_four_list.end();
}

void FourSpaceSearch::GetDependentReachMove(const RelaxedFourID relaxed_four_id, vector<vector<MovePair>> * const move_pair_vector) const
{
  assert(move_pair_vector != nullptr);

  const auto &relaxed_four = relaxed_four_list_[relaxed_four_id];
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

  const auto relaxed_four_id_list = reach_region_[gain_move];
  MoveBitSet cost_move_bit;

  for(const auto relaxed_four_id : relaxed_four_id_list){
    const auto &relaxed_four = relaxed_four_list_[relaxed_four_id];
    const MovePosition cost_move = relaxed_four.GetCostPosition();

    if(cost_move_bit[cost_move]){
      continue;
    }

    cost_move_bit.set(cost_move);
    *cost_move_list += cost_move;
  }
}

}