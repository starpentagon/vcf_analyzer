#ifndef FOUR_SPACE_SEARCH_INL_H
#define FOUR_SPACE_SEARCH_INL_H

#include <set>

#include "FourSpaceSearch.h"

namespace realcore
{

template<PlayerTurn P>
void FourSpaceSearch::ExpandFourSpace(const std::vector<MovePair> &four_list)
{
  std::vector<RelaxedFourID> null_rest_list;
  MoveBitSet gain_bit, cost_bit;

  for(const auto &four : four_list){
    const auto four_attack = four.first;
    const auto four_guard = four.second;

    const RelaxedFourID relaxed_four_id = AddRelaxedFour(four_attack, four_guard, null_rest_list);
    UpdateReachPutRegion<P>(relaxed_four_id, &gain_bit, &cost_bit);
  }
}

template<PlayerTurn P>
inline void FourSpaceSearch::ExpandFourSpace()
{
  static constexpr auto kUpdateFlagFour = (P == kBlackTurn) ? kUpdateFlagFourBlack : kUpdateFlagFourWhite;
  BoardOpenState board_open_state;
  GetBoardOpenState(kUpdateFlagFour, &board_open_state);

  std::vector<MovePair> four_list;
  EnumerateFourMoves<P>(board_open_state, &four_list);

  ExpandFourSpace<P>(four_list);
}

inline void FourSpaceSearch::ExpandFourSpace(const bool is_black_turn)
{
  if(is_black_turn){
    ExpandFourSpace<kBlackTurn>();
  }else{
    ExpandFourSpace<kWhiteTurn>();
  }
}

inline void FourSpaceSearch::ExpandFourSpace(const bool is_black_turn, const std::vector<MovePair> &four_list)
{
  if(is_black_turn){
    ExpandFourSpace<kBlackTurn>(four_list);
  }else{
    ExpandFourSpace<kWhiteTurn>(four_list);
  }
}

template<PlayerTurn P>
void FourSpaceSearch::UpdateReachPutRegion(const RelaxedFourID relaxed_four_id, MoveBitSet * const gain_bit, MoveBitSet * const cost_bit)
{
  assert(relaxed_four_id < relaxed_four_list_.size());
  assert(gain_bit != nullptr);
  assert(cost_bit != nullptr);

  const RelaxedFour &relaxed_four = relaxed_four_list_[relaxed_four_id];

  const MovePosition gain_position = relaxed_four.GetGainPosition();
  const MovePosition cost_position = relaxed_four.GetCostPosition();

  reach_region_[gain_position].emplace_back(relaxed_four_id);
  put_region_[cost_position].emplace_back(relaxed_four_id);

  static constexpr PositionState S = GetPlayerStone(P);
  static constexpr PositionState T = GetPlayerStone(GetOpponentTurn(P));
  
  assert(GetState(gain_position) == kOpenPosition);
  SetState<S>(gain_position);
  gain_bit->set(gain_position);

  assert(GetState(cost_position) == kOpenPosition);
  SetState<T>(cost_position);
  cost_bit->set(cost_position);

  // 新たに四ノビを作れるかチェックする
  LineNeighborhood line_neighborhood(gain_position, kOpenStateNeighborhoodSize, *this);

  static constexpr uint64_t kUpdateFlagFour = P == kBlackTurn ? kUpdateFlagFourBlack : kUpdateFlagFourWhite;
  static constexpr uint64_t kUpdateFlagPointOfSword = P == kBlackTurn ? kUpdateFlagPointOfSwordBlack : kUpdateFlagPointOfSwordWhite;
  static constexpr UpdateOpenStateFlag kUpdateFlag(kUpdateFlagFour | kUpdateFlagPointOfSword);

  BoardOpenState board_open_state;
  line_neighborhood.AddOpenState<P>(kUpdateFlag, &board_open_state);

  std::vector<NextRelaxedFourInfo> next_four_info_list;

  // 到達路 + 自石2つで四ノビ新生
  GetRelaxedFourFromOneGainPosition<P>(board_open_state, &next_four_info_list);

  // 到達路2つ + 自石１つで四ノビ新生
  GetRelaxedFourFromTwoGainPosition<P>(board_open_state, kNullMove, &next_four_info_list);

  // 到達路3つで四ノビ新生
  GetRelaxedFourFromThreeGainPosition<P>(gain_position, &next_four_info_list);

  // 重複する緩和四ノビを削除
  std::sort(next_four_info_list.begin(), next_four_info_list.end());
  next_four_info_list.erase(std::unique(next_four_info_list.begin(), next_four_info_list.end()), next_four_info_list.end());

  std::vector<RelaxedFourID> child_rest_list{relaxed_four_id};

  for(const auto next_four_info : next_four_info_list){
    const MovePosition next_gain = std::get<0>(next_four_info);
    const MovePosition next_cost = std::get<1>(next_four_info);
    const MovePosition rest_max = std::get<2>(next_four_info);
    const MovePosition rest_min = std::get<3>(next_four_info);
    
    if(rest_max == kNullMove && rest_min == kNullMove){
      // 到達路 + 自石2つのため別の到達路を展開する必要はない
      MoveBitSet child_gain_bit = *gain_bit, child_cost_bit = *cost_bit;
      const RelaxedFourID relaxed_four_id = AddRelaxedFour(next_gain, next_cost, child_rest_list);
  
      UpdateReachPutRegion<P>(relaxed_four_id, &child_gain_bit, &child_cost_bit);
      continue;
    }

    std::vector<RestGainFourID> rest_gain_id_list;
    GetRestRelaxedFourID(next_four_info, &rest_gain_id_list);

    for(const auto &rest_gain_id : rest_gain_id_list){
      const RelaxedFourID rest_max_id = rest_gain_id.first;
      const RelaxedFourID rest_min_id = rest_gain_id.second;
      
      MoveBitSet rest_max_gain, rest_max_cost, rest_min_gain, rest_min_cost;

      GetReachableBit(rest_max_id, &rest_max_gain, &rest_max_cost);
      GetReachableBit(rest_min_id, &rest_min_gain, &rest_min_cost);

      // 競合判定
      if(IsConflict(*gain_bit, *cost_bit, rest_max_gain, rest_max_cost)){
        continue;
      }

      if(IsConflict(*gain_bit, *cost_bit, rest_min_gain, rest_min_cost)){
        continue;
      }
      
      if(IsConflict(rest_max_gain, rest_max_cost, rest_min_gain, rest_min_cost)){
        continue;
      }

      // 到達手順を設定
      MoveBitSet rest_gain_bit = (rest_max_gain | rest_min_gain) & ~(*gain_bit);
      MoveBitSet rest_cost_bit = (rest_max_cost | rest_min_cost) & ~(*cost_bit);

      // 到達路と子の獲得路／損失路が競合していないかチェック
      if(rest_gain_bit[next_gain] || rest_gain_bit[next_cost]){
        continue;
      }

      if(rest_cost_bit[next_gain] || rest_cost_bit[next_cost]){
        continue;
      }

      MoveList gain_move_list, cost_move_list;
      GetMoveList(rest_gain_bit, &gain_move_list);
      GetMoveList(rest_cost_bit, &cost_move_list);

      assert(!gain_move_list.empty());
      assert(!cost_move_list.empty());

      for(const auto gain_move : gain_move_list){
        SetState<S>(gain_move);
      }

      for(const auto cost_move : cost_move_list){
        SetState<T>(cost_move);
      }

      MoveBitSet child_gain_bit = *gain_bit | rest_gain_bit, child_cost_bit = *cost_bit | rest_cost_bit;
      std::vector<RelaxedFourID> rest_id_list;

      rest_id_list.emplace_back(relaxed_four_id);

      if(rest_max_id != kInvalidFourID){
        rest_id_list.emplace_back(rest_max_id);
      }

      if(rest_min_id != kInvalidFourID){
        rest_id_list.emplace_back(rest_min_id);
      }

      const RelaxedFourID child_relaxed_four_id = AddRelaxedFour(next_gain, next_cost, rest_id_list);
      UpdateReachPutRegion<P>(child_relaxed_four_id, &child_gain_bit, &child_cost_bit);

      // 到達手順を元に戻す
      for(const auto gain_move : gain_move_list){
        SetState<kOpenPosition>(gain_move);
      }

      for(const auto cost_move : cost_move_list){
        SetState<kOpenPosition>(cost_move);
      }
    }
  }

  SetState<kOpenPosition>(gain_position);
  gain_bit->reset(gain_position);
  SetState<kOpenPosition>(cost_position);
  cost_bit->reset(cost_position);
}

template<PlayerTurn P>
void FourSpaceSearch::GetRelaxedFourFromOneGainPosition(const BoardOpenState &board_open_state, std::vector<NextRelaxedFourInfo> * const next_four_info_list) const
{
  assert(next_four_info_list != nullptr);
  static constexpr OpenStatePattern kPattern = P == kBlackTurn ? kNextFourBlack : kNextFourWhite;
  const auto &pattern_list = board_open_state.GetList(kPattern);

  for(const auto &pattern : pattern_list){
    const auto next_gain = GetBoardMove(pattern.GetOpenPosition());
    const auto next_cost = GetBoardMove(pattern.GetCheckPosition());

    next_four_info_list->emplace_back(
      NextRelaxedFourInfo(next_gain, next_cost, kNullMove, kNullMove)
    );
  }
}

template<PlayerTurn P>
void FourSpaceSearch::GetRelaxedFourFromTwoGainPosition(const BoardOpenState &board_open_state, const MovePosition additional_gain, std::vector<NextRelaxedFourInfo> * const next_four_info_list) const
{
  assert(next_four_info_list != nullptr);
  static constexpr OpenStatePattern kPattern = P == kBlackTurn ? kNextPointOfSwordBlack : kNextPointOfSwordWhite;
  const auto &pattern_list = board_open_state.GetList(kPattern);

  for(const auto &pattern : pattern_list){
    const auto open_rest = GetBoardMove(pattern.GetOpenPosition());

    if(reach_region_[open_rest].empty()){
      continue;
    }

    std::array<BoardPosition, 2> four_position;
    pattern.GetFourPosition(&four_position);

    {
      const auto next_gain = GetBoardMove(four_position[0]);
      const auto next_cost = GetBoardMove(four_position[1]);
      const auto rest_max = std::max(open_rest, additional_gain);
      const auto rest_min = std::min(open_rest, additional_gain);

      next_four_info_list->emplace_back(
        NextRelaxedFourInfo(next_gain, next_cost, rest_max, rest_min)
      );
    }
    {
      const auto next_gain = GetBoardMove(four_position[1]);
      const auto next_cost = GetBoardMove(four_position[0]);
      const auto rest_max = std::max(open_rest, additional_gain);
      const auto rest_min = std::min(open_rest, additional_gain);

      next_four_info_list->emplace_back(
        NextRelaxedFourInfo(next_gain, next_cost, rest_max, rest_min)
      );
    }
  }
}

template<PlayerTurn P>
void FourSpaceSearch::GetRelaxedFourFromThreeGainPosition(const MovePosition gain_position, std::vector<NextRelaxedFourInfo> * const next_four_info_list)
{
  static constexpr PositionState S = GetPlayerStone(P);
  static constexpr uint64_t kUpdateFlagPointOfSword = P == kBlackTurn ? kUpdateFlagPointOfSwordBlack : kUpdateFlagPointOfSwordWhite;

  for(const auto direction : GetBoardDirection()){
    // gainを中心にdirection方向の別の到達路を着手し「到達路2つ + 自石１」で生じる四ノビを求める
    std::vector<MovePosition> additional_gain_list;
    GetRestableGainPositionList(gain_position, direction, &additional_gain_list);
    
    if(additional_gain_list.size() < 2){
      // direction方向にgainを除いて到達路が0 or 1個
      continue;
    }

    for(const auto additional_gain : additional_gain_list){
      SetState<S>(additional_gain);

      LineNeighborhood line_neighborhood(gain_position, kOpenStateNeighborhoodSize, *this);

      BoardOpenState board_open_state;
      line_neighborhood.AddOpenState<P>(kUpdateFlagPointOfSword, &board_open_state);

      // 到達路2つ + 自石１つ(着手した到達路)で四ノビ新生
      GetRelaxedFourFromTwoGainPosition<P>(board_open_state, additional_gain, next_four_info_list);

      SetState<kOpenPosition>(additional_gain);
    }
  }  
}

inline const size_t FourSpaceSearch::GetRelaxedFourCount() const{
  return relaxed_four_list_.size() - 1;
}

}   // namespace realcore


#endif
