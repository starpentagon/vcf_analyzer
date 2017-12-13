#ifndef RELAXED_FOUR_INL_H
#define RELAXED_FOUR_INL_H

#include "BitBoard.h"
#include "FourSpace.h"
#include "RelaxedFour.h"

namespace realcore
{

template<PlayerTurn P>
const RelaxedFourStatus RelaxedFour::GetRelaxedFourStatus(const FourSpace &four_space, const BitBoard &bit_board, MovePair * const opponent_four)
{
  assert(opponent_four != nullptr);

  static constexpr PlayerTurn Q = GetOpponentTurn(P);
  static constexpr PositionState S = GetPlayerStone(P);
  static constexpr PositionState T = GetPlayerStone(Q);

  // local bitsetを生成する(todo cacheして高速化)
  MoveBitSet local_gain_bit, local_cost_bit, local_bit;
  GetLineNeighborhoodBit(gain_, kOpenStateNeighborhoodSize, &local_gain_bit);
  GetLineNeighborhoodBit(cost_, kOpenStateNeighborhoodSize, &local_cost_bit);

  local_bit = local_gain_bit | local_cost_bit;

  FourSpace local_four_space(four_space, local_bit);

  RelaxedFourStatus status = CheckTranspositionTable(local_four_space);

  if(status != kRelaxedFourUnknown){
    return status;
  }

  BitBoard local_bit_board(bit_board);

  MoveList gain_list, cost_list;
  GetMoveList(local_four_space.GetGainBit(), &gain_list);
  GetMoveList(local_four_space.GetCostBit(), &cost_list);

  for(const auto move : gain_list){
    local_bit_board.SetState<S>(move);
  }

  for(const auto move : cost_list){
    local_bit_board.SetState<T>(move);
  }

  // 着手時に五連以上ができるかチェック
  if(status == kRelaxedFourUnknown){
    local_bit_board.SetState<S>(gain_);
    local_bit_board.SetState<T>(cost_);

    const bool is_five = local_bit_board.IsFiveStones<kBlackTurn>() || local_bit_board.IsFiveStones<kWhiteTurn>();

    if(is_five){
      status = kRelaxedFourInfeasible;
    }

    local_bit_board.SetState<kOpenPosition>(gain_);
    local_bit_board.SetState<kOpenPosition>(cost_);
  }

  // 防御不可能な四ノリが発生していないかチェックする
/*
  if(status == kRelaxedFourUnknown){
    if(!CanGuardOpponentFour<P>(four_space, bit_board)){
      status = kRelaxedFourInfeasible;
    }
  }
*/
  // 四々/三々チェックを行う
  if(status == kRelaxedFourUnknown){
    if(local_bit_board.IsForbiddenMove<P>(gain_)){
      status = kRelaxedFourDblFourThree;
    }
  }
  
  // 終端チェックを行う
  // todo 白番にも対応(黒の「五連以上」と「長連」を分けて判定する必要あり)
  if(status == kRelaxedFourUnknown){
    if(local_bit_board.IsOpenFourMove<P>(gain_)){
      status = kRelaxedFourTerminate;
    }
  }

  // R-四ノビが成立するかチェックする
  MovePosition guard_move;
  
  if(status == kRelaxedFourUnknown){
    if(!local_bit_board.IsFourMove<P>(gain_, &guard_move)){
      status = kRelaxedFourFail;
    }else if(cost_ != guard_move){
      status = kRelaxedFourFail;
    }
  }

  // 四ノリが発生しているかチェックする
  if(status == kRelaxedFourUnknown){
    local_bit_board.SetState<S>(gain_);
    
    MovePosition opponent_guard;
    if(local_bit_board.IsFourMove<Q>(cost_, &opponent_guard)){
      status = kRelaxedFourOpponentFour;

      opponent_four->first = cost_;
      opponent_four->second = opponent_guard;

      if(!CanGuardOpponentFour<P>(four_space, *opponent_four, bit_board)){
        status = kRelaxedFourInfeasible;
      }
    }

    local_bit_board.SetState<kOpenPosition>(gain_);
  }

  // R-四ノビ可
  if(status == kRelaxedFourUnknown){
    status = kRelaxedFourFeasible;
  }

  transposition_table_.emplace_back(std::make_pair(local_four_space, status));
  return status;
}

template<PlayerTurn P>
const bool RelaxedFour::CanGuardOpponentFour(const FourSpace &four_space, const BitBoard &bit_board) const
{
  // todo implementation for white
  if(P == kWhiteTurn){
    return true;
  }

  const auto opponent_four_list = four_space.GetOpponentFourList();

  if(opponent_four_list.empty()){
    return true;
  }

  for(const auto &move_pair : opponent_four_list){
    if(!CanGuardOpponentFour<P>(four_space, move_pair, bit_board)){
      return false;
    }
  }

  return true;
}

template<PlayerTurn P>
const bool RelaxedFour::CanGuardOpponentFour(const FourSpace &four_space, const MovePair &move_pair, const BitBoard &bit_board) const
{
  static constexpr PlayerTurn Q = GetOpponentTurn(P);
  static constexpr PositionState S = GetPlayerStone(P);
  static constexpr PositionState T = GetPlayerStone(Q);

  const auto opponent_four_move = move_pair.first;
  const auto guard_move = move_pair.second;

  MoveBitSet local_gain_bit, local_cost_bit, local_bit;
  GetLineNeighborhoodBit(guard_move, kOpenStateNeighborhoodSize, &local_gain_bit);
  GetLineNeighborhoodBit(opponent_four_move, kOpenStateNeighborhoodSize, &local_cost_bit);

  local_bit = local_gain_bit | local_cost_bit;
  
  FourSpace local_four_space(four_space, local_bit);
  BitBoard local_bit_board(bit_board);

  MoveList gain_list, cost_list;
  GetMoveList(local_four_space.GetGainBit(), &gain_list);
  GetMoveList(local_four_space.GetCostBit(), &cost_list);
  
  for(const auto move : gain_list){
    local_bit_board.SetState<S>(move);
  }

  local_bit_board.SetState<S>(gain_);

  for(const auto move : cost_list){
    local_bit_board.SetState<T>(move);
  }

  local_bit_board.SetState<T>(cost_);
  
  // 四ノリが生じているかチェックする
  MovePosition guard;
  
  if(!local_bit_board.IsFourMoveOnBoard<Q>(opponent_four_move, &guard)){
    return true;
  }

  if(guard != guard_move){
    return true;
  }

  // 四ノリ防手位置で四ノリ返せる可能性があるかチェックする
  bool is_refour = false;
 
  for(BoardDirection direction : GetBoardDirection()){
    const auto board_position = GetBoardPosition(guard_move, direction);
    BoardPosition max_board_position, min_board_position;

    for(size_t i=1; i<5; i++){
      const auto check_position = board_position + i;
      max_board_position = check_position;
      
      if(!IsInBoard(check_position) || local_bit_board.GetState(check_position) == T){
        max_board_position = check_position - 1;
        break;
      }
    }  

    for(size_t i=1; i<5; i++){
      const auto check_position = board_position - i;
      min_board_position = check_position;
      
      if(!IsInBoard(check_position) || local_bit_board.GetState(check_position) == T){
        min_board_position = check_position + 1;
        break;
      }
    }

    size_t distance = max_board_position - min_board_position;

    if(distance >= 4){
      is_refour = true;
      break;
    }
  }

  if(is_refour){
    return true;
  }

  return false;
}

}   // namespace realcore

#endif    // RELAXED_FOUR_INL_H
