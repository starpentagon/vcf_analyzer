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

  // local bitsetを生成する
  const MoveBitSet &local_gain_bit = GetLineNeighborhoodBit<kOpenStateNeighborhoodSize>(gain_);
  const MoveBitSet &local_cost_bit = GetLineNeighborhoodBit<kOpenStateNeighborhoodSize>(cost_);
  
  MoveBitSet local_bit = local_gain_bit | local_cost_bit;

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

  assert(!local_bit_board.IsFiveStones<kBlackTurn>());
  assert(!local_bit_board.IsFiveStones<kWhiteTurn>());

  // 獲得路を着手した時に攻め方に五連以上ができる場合は親ノードで終端 or 長連を自ら打つ場合なので実現不可能
  if(status == kRelaxedFourUnknown){
    const bool is_five = local_bit_board.IsFiveStones<P>(gain_);

    if(is_five){
      status = kRelaxedFourInfeasible;
    }
  }

  // 獲得路 -> 損失路を着手した時に受け方に
  // (i)五連が生じている -> 実現不可能
  // (ii)長連が生じている -> 終端(白番のみ)
  if(status == kRelaxedFourUnknown){
    local_bit_board.SetState<S>(gain_);

    const bool is_overline = Q == kBlackTurn && local_bit_board.IsOverline<kBlackTurn>(cost_);
    const bool is_five = (!is_overline && local_bit_board.IsFiveStones<Q>(cost_));
    
    local_bit_board.SetState<kOpenPosition>(gain_);

    if(is_five){
      status = kRelaxedFourInfeasible;
    }else if(is_overline){
      status = kRelaxedFourTerminate;
    }
  }

  // @note 防御不可能な四ノリが発生していないかのチェックは計算量が多く不採用

  // 長連のチェックはすでに行っているため四々/三々チェックを行う
  if(P == kBlackTurn && status == kRelaxedFourUnknown){
    if(local_bit_board.IsDoubleFourMove<P>(gain_) || local_bit_board.IsDoubleSemiThreeMove<P>(gain_)){
      status = kRelaxedFourDblFourThree;
    }
  }
  
  // 終端チェックを行う(黒/白共通)
  if(status == kRelaxedFourUnknown){
    if(local_bit_board.IsOpenFourMove<P>(gain_)){
      status = kRelaxedFourTerminate;
    }
  }

  // 終端チェックを行う(白番のみ)
  if(P == kWhiteTurn && status == kRelaxedFourUnknown){
    if(local_bit_board.IsDoubleFourMove<P>(gain_)){
      status = kRelaxedFourTerminate;
    }

    local_bit_board.SetState<S>(gain_);

    if(local_bit_board.IsDoubleFourMove<Q>(cost_) || local_bit_board.IsDoubleSemiThreeMove<Q>(cost_)){
      status = kRelaxedFourTerminate;
    }

    local_bit_board.SetState<kOpenPosition>(gain_);
  }

  // 黒番の場合、長連筋になることがあるためR-四ノビが成立するかチェックする
  MovePosition guard_move;

  // todo delete --
  if(P == kWhiteTurn && status == kRelaxedFourUnknown){
    if(!local_bit_board.IsFourMove<P>(gain_, &guard_move)){
      assert(false);
    }else if(cost_ != guard_move){
      assert(false);
    }
  }
  // -- todo delete
  
  if(P == kBlackTurn && status == kRelaxedFourUnknown){
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
const bool RelaxedFour::CanGuardOpponentFour(const FourSpace &four_space, const MovePair &move_pair, const BitBoard &bit_board) const
{
  static constexpr PlayerTurn Q = GetOpponentTurn(P);
  static constexpr PositionState S = GetPlayerStone(P);
  static constexpr PositionState T = GetPlayerStone(Q);

  const auto opponent_four_move = move_pair.first;
  const auto guard_move = move_pair.second;

  const MoveBitSet &local_gain_bit = GetLineNeighborhoodBit<kOpenStateNeighborhoodSize>(guard_move);
  const MoveBitSet &local_cost_bit = GetLineNeighborhoodBit<kOpenStateNeighborhoodSize>(guard_move);

  MoveBitSet local_bit = local_gain_bit | local_cost_bit;
  
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

inline const MovePosition RelaxedFour::GetGainPosition() const{
  return gain_;
}

inline const MovePosition RelaxedFour::GetCostPosition() const{
  return cost_;
}

inline const OpenRestList& RelaxedFour::GetOpenRestList() const{
  return open_rest_list_;
}

inline const RelaxedFourStatusTable& RelaxedFour::GetTranspositionTable() const{
  return transposition_table_;
}

}   // namespace realcore

#endif    // RELAXED_FOUR_INL_H
