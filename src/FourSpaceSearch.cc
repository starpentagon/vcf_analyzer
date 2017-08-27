#include <set>  // todo delete

#include "MoveTree.h"   // todo delete
#include "HashTable.h"   // todo delete
#include "FourSpaceSearch.h"

using namespace std;

namespace realcore{

FourSpaceSearch::FourSpaceSearch(const BitBoard &bit_board)
: BitBoard(bit_board), attack_player_(kBlackTurn)
{
  vector<RelaxedFourID> null_rest_list;
  
  // kInvalidFourID用にダミーデータを設定する
  relaxed_four_list_.emplace_back(
    kNullMove, kNullMove, null_rest_list
  );

  relaxed_four_conflict_flag_.push_back(0);
}

const RelaxedFourID FourSpaceSearch::AddRelaxedFour(const MovePosition gain_position, const MovePosition cost_position, const std::vector<RelaxedFourID> &rest_list)
{
  relaxed_four_list_.emplace_back(
    gain_position, cost_position, rest_list
  );

  relaxed_four_conflict_flag_.push_back(0);

  const RelaxedFourID four_id = relaxed_four_list_.size() - 1;
  
  MoveList move_list;
  GetReachSequence(four_id, &move_list);
  const auto hash_value = CalcHashValue(move_list);

  const auto find_it = relaxed_four_transposition_map_.find(hash_value);

  if(find_it != relaxed_four_transposition_map_.end()){
    // 登録済みの場合
    relaxed_four_list_.pop_back();
    relaxed_four_conflict_flag_.pop_back();

    return find_it->second;
  }

  relaxed_four_transposition_map_.insert(pair<HashValue, RelaxedFourID>(hash_value, four_id));

  return four_id;
}

const RelaxedFourID FourSpaceSearch::AddRelaxedFour(const RelaxedFour &relaxed_four)
{
  relaxed_four_list_.emplace_back(relaxed_four);
  relaxed_four_conflict_flag_.push_back(0);
  
  const RelaxedFourID four_id = relaxed_four_list_.size() - 1;
  
  MoveList move_list;
  GetReachSequence(four_id, &move_list);
  const auto hash_value = CalcHashValue(move_list);

  const auto find_it = relaxed_four_transposition_map_.find(hash_value);

  if(find_it != relaxed_four_transposition_map_.end()){
    // 登録済みの場合
    relaxed_four_list_.pop_back();
    relaxed_four_conflict_flag_.pop_back();

    return find_it->second;
  }

  relaxed_four_transposition_map_.insert(pair<HashValue, RelaxedFourID>(hash_value, four_id));

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

void FourSpaceSearch::GetRestRelaxedFourID(const NextRelaxedFourInfo &next_four_info, vector<RestGainFourID> * const rest_gain_id_list) const
{
  assert(rest_gain_id_list != nullptr);

  const MovePosition rest_max = get<2>(next_four_info);
  const MovePosition rest_min = get<3>(next_four_info);
  
  assert(rest_max != kNullMove);

  for(const auto rest_max_id : reach_region_[rest_max]){
    if(relaxed_four_conflict_flag_[rest_max_id] == 1){
      continue;
    }

    if(rest_min == kNullMove){
      rest_gain_id_list->emplace_back(RestGainFourID(rest_max_id, kInvalidFourID));
      continue;
    }

    for(const auto rest_min_id : reach_region_[rest_min]){
      if(relaxed_four_conflict_flag_[rest_min_id] == 1){
        continue;
      }

      rest_gain_id_list->emplace_back(rest_max_id, rest_min_id);
    }
  }
}

void FourSpaceSearch::GetReachIDSequence(const RelaxedFourID relaxed_four_id, vector<RelaxedFourID> * const id_list) const
{
  set<RelaxedFourID> id_set;
  GetReachIDSequence(relaxed_four_id, &id_set, id_list);
}

void FourSpaceSearch::GetReachIDSequence(const RelaxedFourID relaxed_four_id, set<RelaxedFourID> * const appeared_id_set, vector<RelaxedFourID> * const id_list) const
{
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
  vector<RelaxedFourID> four_id_list;
  GetReachIDSequence(relaxed_four_id, &four_id_list);
  return GetReachSequence(four_id_list, move_list);
}

bool FourSpaceSearch::GetReachSequence(const vector<RelaxedFourID> &four_id_list, MoveList * const move_list) const{
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

}