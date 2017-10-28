#ifndef FOUR_SPACE_SEARCH_INL_H
#define FOUR_SPACE_SEARCH_INL_H

#include <set>

#include "FourSpaceSearch.h"

namespace realcore
{

template<PlayerTurn P>
void FourSpaceSearch::ExpandFourSpace(const std::vector<MovePair> &four_list)
{
  attack_player_ = P;
  std::vector<MovePosition> null_rest_list;

  for(const auto &four : four_list){
    const auto four_attack = four.first;
    const auto four_guard = four.second;

    const RelaxedFourID relaxed_four_id = AddRelaxedFour(four_attack, four_guard, null_rest_list);
    UpdateReachPutRegion<P>(relaxed_four_id);
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
void FourSpaceSearch::UpdateReachPutRegion(const RelaxedFourID relaxed_four_id)
{
  assert(relaxed_four_id < relaxed_four_list_.size());

  const RelaxedFour &relaxed_four = relaxed_four_list_[relaxed_four_id];

  if(IsExpanded(relaxed_four_id)){
    return;
  }

  const MovePosition gain_position = relaxed_four.GetGainPosition();
  const MovePosition cost_position = relaxed_four.GetCostPosition();

  reach_region_[gain_position].emplace_back(relaxed_four_id);
  put_region_[cost_position].emplace_back(relaxed_four_id);

  static constexpr PositionState S = GetPlayerStone(P);
  static constexpr PositionState T = GetPlayerStone(GetOpponentTurn(P));
  
  // todo delete
  static size_t count = 0;

  if(++count % 10 == 0){
    std::cerr << count << std::endl;
    std::cerr << "\tR-four count: " << GetRelaxedFourCount() << std::endl;
    std::cerr << "\tR-four length: " << GetMaxRelaxedFourLength() << std::endl;

    for(const auto move : GetAllInBoardMove()){
      Cordinate x, y;
      GetMoveCordinate(move, &x, &y);

      std::cerr << reach_region_[move].size() << ",";

      if(x == 15){
        std::cerr << std::endl;
      }
    }
  }

  std::vector<std::vector<MovePair>> move_pair_vector;
  GetDependentReachMove(relaxed_four_id, &move_pair_vector);

  for(const auto &move_pair_list : move_pair_vector){
    for(const auto move_pair : move_pair_list){
      const auto gain_position = move_pair.first;
      assert(GetState(gain_position) == kOpenPosition);
      SetState<S>(gain_position);
    
      const auto cost_position = move_pair.second;
      assert(GetState(cost_position) == kOpenPosition);
      SetState<T>(cost_position);
    }

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
  
    for(const auto move_pair : move_pair_list){
      const auto gain_position = move_pair.first;
      const auto cost_position = move_pair.second;

      SetState<kOpenPosition>(gain_position);
      SetState<kOpenPosition>(cost_position);  
    }

    for(const auto next_four_info : next_four_info_list){
      const MovePosition next_gain = std::get<0>(next_four_info);
      const MovePosition next_cost = std::get<1>(next_four_info);
      const MovePosition rest_max = std::get<2>(next_four_info);
      const MovePosition rest_min = std::get<3>(next_four_info);
  
      std::vector<MovePosition> rest_list{gain_position};
      rest_list.reserve(3);
  
      if(rest_max != kNullMove){
        rest_list.emplace_back(rest_max);
      }
  
      if(rest_min != kNullMove){
        rest_list.emplace_back(rest_min);
      }
      
      const RelaxedFourID child_relaxed_four_id = AddRelaxedFour(next_gain, next_cost, rest_list);
      UpdateReachPutRegion<P>(child_relaxed_four_id);
    }
  }
}

template<PlayerTurn P>
void FourSpaceSearch::GetRelaxedFourFromOneGainPosition(const BoardOpenState &board_open_state, std::vector<NextRelaxedFourInfo> * const next_four_info_list) const
{
  assert(next_four_info_list != nullptr);

  // X[B3O2]X, [W3O2]を検索する
  static constexpr OpenStatePattern kPattern = P == kBlackTurn ? kNextFourBlack : kNextFourWhite;
  const auto &pattern_list = board_open_state.GetList(kPattern);

  for(const auto &pattern : pattern_list){
    const auto next_gain = GetBoardMove(pattern.GetOpenPosition());
    const auto next_cost = GetBoardMove(pattern.GetCheckPosition());

    next_four_info_list->emplace_back(
      next_gain, next_cost, kNullMove, kNullMove
    );
  }
}

template<PlayerTurn P>
void FourSpaceSearch::GetRelaxedFourFromTwoGainPosition(const BoardOpenState &board_open_state, const MovePosition additional_gain, std::vector<NextRelaxedFourInfo> * const next_four_info_list) const
{
  assert(next_four_info_list != nullptr);
  
  // X[B2O3]X, [W2O3]を検索する
  static constexpr OpenStatePattern kPattern = P == kBlackTurn ? kNextPointOfSwordBlack : kNextPointOfSwordWhite;
  const auto &pattern_list = board_open_state.GetList(kPattern);

  for(const auto &pattern : pattern_list){
    // 到達可能な空点かチェックする
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
        next_gain, next_cost, rest_max, rest_min
      );
    }
    {
      const auto next_gain = GetBoardMove(four_position[1]);
      const auto next_cost = GetBoardMove(four_position[0]);
      const auto rest_max = std::max(open_rest, additional_gain);
      const auto rest_min = std::min(open_rest, additional_gain);

      next_four_info_list->emplace_back(
        next_gain, next_cost, rest_max, rest_min
      );
    }
  }
}

template<PlayerTurn P>
void FourSpaceSearch::GetRelaxedFourFromThreeGainPosition(const MovePosition gain_position, std::vector<NextRelaxedFourInfo> * const next_four_info_list)
{
  static constexpr PositionState S = GetPlayerStone(P);
  static constexpr PositionState T = GetPlayerStone(GetOpponentTurn(P));
  static constexpr uint64_t kUpdateFlagPointOfSword = P == kBlackTurn ? kUpdateFlagPointOfSwordBlack : kUpdateFlagPointOfSwordWhite;

  for(const auto direction : GetBoardDirection()){
    // gainを中心にdirection方向の別の到達路を着手し「到達路2つ + 自石１」で生じる四ノビを求める
    std::vector<RelaxedFourID> additional_four_id_list;
    const size_t open_rest_count = GetRestableRelaxedFourIDList(gain_position, direction, &additional_four_id_list);
    
    if(open_rest_count < 2){
      // direction方向にgainを除いて到達路が0 or 1個
      continue;
    }

    for(const auto additional_four_id : additional_four_id_list){
      const auto &relaxed_four = relaxed_four_list_[additional_four_id];
      const auto additional_gain = relaxed_four.GetGainPosition();
      const auto additional_cost = relaxed_four.GetCostPosition();

      if(GetState(additional_gain) != kOpenPosition || GetState(additional_cost) != kOpenPosition){
        continue;
      }

      SetState<S>(additional_gain);
      SetState<T>(additional_cost);

      LineNeighborhood line_neighborhood(gain_position, kOpenStateNeighborhoodSize, *this);

      BoardOpenState board_open_state;
      line_neighborhood.AddOpenState<P>(kUpdateFlagPointOfSword, direction, &board_open_state);

      // 到達路2つ + 自石１つ(着手した到達路)で四ノビ新生
      GetRelaxedFourFromTwoGainPosition<P>(board_open_state, additional_gain, next_four_info_list);

      SetState<kOpenPosition>(additional_gain);
      SetState<kOpenPosition>(additional_cost);
    }
  }  
}

inline const size_t FourSpaceSearch::GetRelaxedFourCount() const{
  return relaxed_four_list_.size() - 1;
}

template<PositionState S>
inline void FourSpaceSearch::SetState(const MovePosition move)
{
  if(S == kOpenPosition){
    --search_sequence_;
  }else{
    search_sequence_ += move;
  }

  BitBoard::SetState<S>(move);
}


}   // namespace realcore


#endif
