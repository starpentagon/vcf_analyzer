#ifndef RELAXED_FOUR_INL_H
#define RELAXED_FOUR_INL_H

#include "BitBoard.h"
#include "FourSpace.h"
#include "RelaxedFour.h"

namespace realcore
{

template<PlayerTurn P>
const RelaxedFourStatus RelaxedFour::GetRelaxedFourStatus(const FourSpace &four_space, const BitBoard &bit_board)
{
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
    
    if(local_bit_board.IsFourMove<Q>(cost_, nullptr)){
      status = kRelaxedFourOpponentFour;
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



}   // namespace realcore

#endif    // RELAXED_FOUR_INL_H
