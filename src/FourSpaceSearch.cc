#include "FourSpaceSearch.h"

using namespace std;

namespace realcore{

FourSpaceSearch::FourSpaceSearch(const BitBoard &bit_board)
: BitBoard(bit_board)
{
  vector<RelaxedFourID> null_rest_list;
  
  // kInvalidFourID用にダミーデータを設定する
  relaxed_four_list_.emplace_back(
    RelaxedFour(kNullMove, kNullMove, null_rest_list)
  );
}

const RelaxedFourID FourSpaceSearch::AddRelaxedFour(const MovePosition gain_position, const MovePosition cost_position, const std::vector<RelaxedFourID> &rest_list)
{
  relaxed_four_list_.emplace_back(
    RelaxedFour(gain_position, cost_position, rest_list)
  );

  return relaxed_four_list_.size() - 1;
}

const RelaxedFourID FourSpaceSearch::AddRelaxedFour(const RelaxedFour &relaxed_four)
{
  relaxed_four_list_.emplace_back(relaxed_four);

  return relaxed_four_list_.size() - 1;
}

void FourSpaceSearch::GetRestableGainPositionList(const MovePosition gain_position, const BoardDirection direction, std::vector<MovePosition> * const restable_gain_list) const
{
  assert(restable_gain_list != nullptr);
  const BoardPosition gain_board_position = GetBoardPosition(gain_position, direction);

  for(size_t i=1; i<5; i++){
    const auto check_position = gain_board_position + i;

    if(!IsInBoard(check_position)){
      break;
    }

    if(GetState(check_position) != kOpenPosition){
      continue;
    }

    const auto check_move = GetBoardMove(check_position);

    if(!reach_region_[check_move].empty()){
      restable_gain_list->emplace_back(check_move);
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

    if(!reach_region_[check_move].empty()){
      restable_gain_list->emplace_back(check_move);
    }
  }  
}

void FourSpaceSearch::GetRestRelaxedFourID(const NextRelaxedFourInfo &next_four_info, vector<RestGainFourID> * const rest_gain_id_list) const
{
  assert(rest_gain_id_list != nullptr);

  const MovePosition rest_max = get<2>(next_four_info);
  const MovePosition rest_min = get<3>(next_four_info);
  
  assert(rest_max != kNullMove);

  for(const auto rest_max_id : reach_region_[rest_max]){
    if(rest_min == kNullMove){
      rest_gain_id_list->emplace_back(RestGainFourID(rest_max_id, kInvalidFourID));
      continue;
    }

    for(const auto rest_min_id : reach_region_[rest_min]){
      rest_gain_id_list->emplace_back(RestGainFourID(rest_max_id, rest_min_id));
    }
  }
}

void FourSpaceSearch::GetReachableBit(const RelaxedFourID relaxed_four_id, MoveBitSet * const gain_bit, MoveBitSet * const cost_bit) const
{
  assert(gain_bit != nullptr);
  assert(cost_bit != nullptr);
  
  if(relaxed_four_id == kInvalidFourID){
    return;
  }

  const RelaxedFour &relaxed_four = relaxed_four_list_[relaxed_four_id];

  gain_bit->set(relaxed_four.GetGainPosition());
  cost_bit->set(relaxed_four.GetCostPosition());

  const auto &rest_four_id_list = relaxed_four.GetRestPositionList();

  for(const auto rest_four_id : rest_four_id_list){
    GetReachableBit(rest_four_id, gain_bit, cost_bit);
  }
}

const bool FourSpaceSearch::IsConflict(const MoveBitSet &gain_bit_1, const MoveBitSet &cost_bit_1, const MoveBitSet &gain_bit_2, const MoveBitSet &cost_bit_2)
{
  if((gain_bit_1 & cost_bit_2).any()){
    return true;
  }

  if((cost_bit_1 & gain_bit_2).any()){
    return true;
  }

  if((cost_bit_1 & cost_bit_2).any()){
    return true;
  }

  return false;
}

const size_t FourSpaceSearch::GetMaxRelaxedFourLength() const
{
  size_t max_length = 0;

  for(size_t i=1, size=relaxed_four_list_.size(); i<size; i++){
    MoveBitSet gain_bit, cost_bit;
    
    GetReachableBit(i, &gain_bit, &cost_bit);
    const size_t length = gain_bit.count();
    max_length = max(max_length, length);
  }

   return max_length;
}

}