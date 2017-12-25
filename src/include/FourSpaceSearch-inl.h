#ifndef FOUR_SPACE_SEARCH_INL_H
#define FOUR_SPACE_SEARCH_INL_H

#include <set>

#include "FourSpaceSearch.h"

namespace realcore
{

template<PlayerTurn P>
void FourSpaceSearch::ExpandFourSpace(const std::vector<MovePair> &four_list)
{
  for(const auto &four : four_list){
    const auto four_attack = four.first;
    const auto four_guard = four.second;

    FourSpace four_space(four_attack, four_guard);
    NextRelaxedFourInfo next_four_info(four_attack, four_guard, kNullMove, kNullMove, kNullMove);

    const auto result = AddRelaxedFour(next_four_info);
    AddFeasibleRelaxedFourID(result.first);

    AddFourSpace<P>(four_attack, four_guard, four_space);
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
void FourSpaceSearch::AddFourSpace(const MovePosition gain_move, const MovePosition cost_move, const FourSpace &four_space)
{
  // todo delete
  static size_t count = 0;
  count++;

  /*
  if(count >= 1 * 10000){
    return;
  }
  */
  
  // todo delete
  static size_t path_through_matrix[256] = {0};

  {
    MoveList gain_list;
    GetMoveList(four_space.GetGainBit(), &gain_list);

    for(const auto move : gain_list){
      path_through_matrix[move]++;
    }    
  }
/*  
  if(count % 10000 == 0){
    std::cerr << "\tPath through on move" << std::endl;

    for(const auto move : GetAllInBoardMove()){
      Cordinate x, y;
      GetMoveCordinate(move, &x, &y);
  
      std::cerr << path_through_matrix[move] << ",";
  
      if(x == 15){
        std::cerr << std::endl;
      }
    }
  }
*/
  TenYearsFeverCheck(gain_move, four_space);

  // todo delete
  static size_t check_count = 0;

  // todo delete
  if(++check_count % 100000 == 0){
    std::cerr << "add four space check / count = " << 1.0 * check_count / count << "\t" << check_count << " / " << count << std::endl;
    std::cerr << "\tR-four count: " << GetRelaxedFourCount() << std::endl;
    std::cerr << "\tR-four length: " << GetMaxRelaxedFourLength() << std::endl;

    std::cerr << "\tR-four count on move" << std::endl;
    ShowBoardRelaxedFourCount();

    std::cerr << "\tFourSpace count on move" << std::endl;
    ShowBoardFourSpaceCount();
  }

  // 位置moveを残路に持つ緩和四ノビごとに獲得/損失空間を追加できるかチェックする
  std::vector<RestKeyFourSpace> added_four_space_list;
  four_space_manager_.AddFourSpace<P>(gain_move, cost_move, four_space, &added_four_space_list);

  if(added_four_space_list.empty()){
    return;
  }

  UpdateAdditionalPuttableFourSpace<P>(gain_move, four_space, added_four_space_list);

  // 位置moveの直線近傍から新たに緩和四ノビを作れるかチェックする
  std::vector<RelaxedFourID> relaxed_four_id_list;
  GenerateRelaxedFour<P>(gain_move, four_space, &relaxed_four_id_list);

  for(const auto relaxed_four_id : relaxed_four_id_list){
    auto& relaxed_four = GetRelaxedFour(relaxed_four_id);
    const auto& rest_move_list = relaxed_four.GetOpenRestList();

    OpenRestList open_rest_list(rest_move_list);
    std::vector<FourSpaceID> rest_four_space_list;
    four_space_manager_.EnumeratePuttableFourSpace<P>(open_rest_list, &rest_four_space_list);

    ExpandRelaxedFour<P>(relaxed_four_id, rest_four_space_list);
  }
}

template<PlayerTurn P>
void FourSpaceSearch::ExpandRelaxedFour(const RelaxedFourID relaxed_four_id, const std::vector<FourSpaceID> &rest_four_space_list)
{
  auto& relaxed_four = GetRelaxedFour(relaxed_four_id);
  const auto gain_position = relaxed_four.GetGainPosition();
  const auto cost_position = relaxed_four.GetCostPosition();

  for(const auto& rest_four_space_id : rest_four_space_list){
    const auto& rest_four_space = four_space_manager_.GetFourSpace(rest_four_space_id);
    if(rest_four_space.IsConflict(gain_position, cost_position)){
      continue;
    }

    const BitBoard& bit_board = *this;
    MovePair opponent_four;
    const auto relaxed_four_status = relaxed_four.GetRelaxedFourStatus<P>(rest_four_space, bit_board, &opponent_four);

    // todo delete --
    static std::array<size_t, 100> status_count_array{{0}};
    static size_t status_count = 0;

    status_count_array[relaxed_four_status]++;
    status_count++;

    if(status_count % 10000 == 0){
      std::cerr << "kRelaxedFourUnknown: " << status_count_array[kRelaxedFourUnknown] << std::endl;
      std::cerr << "kRelaxedFourInfeasible: " << status_count_array[kRelaxedFourInfeasible] << std::endl;
      std::cerr << "kRelaxedFourFail: " << status_count_array[kRelaxedFourFail] << std::endl;
      std::cerr << "kRelaxedFourDblFourThree: " << status_count_array[kRelaxedFourDblFourThree] << std::endl;
      std::cerr << "kRelaxedFourTerminate: " << status_count_array[kRelaxedFourTerminate] << std::endl;
      std::cerr << "kRelaxedFourOpponentFour: " << status_count_array[kRelaxedFourOpponentFour] << std::endl;
      std::cerr << "kRelaxedFourFeasible: " << status_count_array[kRelaxedFourFeasible] << std::endl;
      std::cerr << std::endl;
    }

    // -- todo delete

    if(relaxed_four_status == kRelaxedFourTerminate){
      FourSpace child_four_space(gain_position, cost_position);
      child_four_space.Add(rest_four_space);
      
      AddFeasibleRelaxedFourID(relaxed_four_id);

      std::vector<RestKeyFourSpace> additional_four_space;
      four_space_manager_.AddFourSpace<P>(gain_position, cost_position, child_four_space, &additional_four_space);

      continue;
    }

    if(!relaxed_four.IsExpandable(relaxed_four_status)){
      continue;
    }

    FourSpace child_four_space(gain_position, cost_position);
    child_four_space.Add(rest_four_space);

    AddFeasibleRelaxedFourID(relaxed_four_id);

    if(relaxed_four_status == kRelaxedFourOpponentFour){
      child_four_space.SetOpponentFour(opponent_four);
    }

    AddFourSpace<P>(gain_position, cost_position, child_four_space);
  }
}

template<PlayerTurn P>
void FourSpaceSearch::UpdateAdditionalPuttableFourSpace(const MovePosition move, const FourSpace &four_space, const std::vector<RestKeyFourSpace> &additional_four_space)
{
  for(const auto& additional : additional_four_space){
    const auto rest_key = additional.first;

    // rest_keyと同一の開残路を持つ緩和四ノビIDを取得
    const auto find_it = rest_list_relaxed_four_list_.find(rest_key);

    if(find_it == rest_list_relaxed_four_list_.end()){
      continue;
    }

    const auto additional_four_space_id = additional.second;
    std::vector<FourSpaceID> additional_four_space_list{additional_four_space_id};

    const auto& relaxed_four_id_list_ptr = find_it->second;
    const std::vector<RelaxedFourID> relaxed_four_id_list = *relaxed_four_id_list_ptr;  // rest_list_relaxed_four_list_が拡張される可能性があるためコピーを作成

    for(const auto relaxed_four_id : relaxed_four_id_list){
      ExpandRelaxedFour<P>(relaxed_four_id, additional_four_space_list);
    }
  }
}

template<PlayerTurn P>
void FourSpaceSearch::GenerateRelaxedFour(const MovePosition gain_position, const FourSpace &four_space, std::vector<RelaxedFourID> * const additional_relaxed_four)
{
  assert(additional_relaxed_four != nullptr);

  static constexpr PlayerTurn Q = GetOpponentTurn(P);
  static constexpr PositionState S = GetPlayerStone(P);
  static constexpr PositionState T = GetPlayerStone(Q);

  // todo delete
  static size_t count = 0;
  
  if(++count % 10000 == 0){
    std::cerr << "GenerateRelaxedFour: " << count << std::endl;
  }

  // 追加する獲得/損失空間と直線近傍のANDパターンを生成
  const MoveBitSet &neighborhood_bit = GetLineNeighborhoodBit<kOpenStateNeighborhoodSize>(gain_position);

  const auto &neighbor_gain_bit = four_space.GetNeighborhoodGainBit(neighborhood_bit);
  const auto &neighbor_cost_bit = four_space.GetNeighborhoodCostBit(neighborhood_bit);

  SetState<S>(neighbor_gain_bit);
  SetState<T>(neighbor_cost_bit);

  LineNeighborhood line_neighborhood(gain_position, kOpenStateNeighborhoodSize, *this);

  // すでに生成済みかチェックする
  LocalBitBoard local_bit_board;
  line_neighborhood.GetLocalBitBoard(&local_bit_board);

  if(IsRegisteredLocalBitBoard(gain_position, local_bit_board)){
    SetState<kOpenPosition>(neighbor_gain_bit | neighbor_cost_bit);

    return;
  }

  move_local_bitboard_list_[gain_position].emplace_back(local_bit_board);

  // 新たに四ノビを作れるかチェックする
  static constexpr uint64_t kUpdateFlagFour = P == kBlackTurn ? kUpdateFlagFourBlack : kUpdateFlagFourWhite;
  static constexpr uint64_t kUpdateFlagPointOfSword = P == kBlackTurn ? kUpdateFlagPointOfSwordBlack : kUpdateFlagPointOfSwordWhite;
  static constexpr UpdateOpenStateFlag kUpdateFlag(kUpdateFlagFour | kUpdateFlagPointOfSword);

  BoardOpenState board_open_state;
  line_neighborhood.AddOpenState<P>(kUpdateFlag, &board_open_state);

  std::vector<NextRelaxedFourInfo> next_four_info_list;

  // 獲得路 + 自石2つで四ノビ新生
  GetRelaxedFourFromOneGainPosition<P>(board_open_state, &next_four_info_list);

  // 獲得路2つ + 自石１つで四ノビ新生
  GetRelaxedFourFromTwoGainPosition<P>(board_open_state, kNullMove, &next_four_info_list);

  // 獲得路3つで四ノビ新生
  GetRelaxedFourFromThreeGainPosition<P>(gain_position, &next_four_info_list);
  
  SetState<kOpenPosition>(neighbor_gain_bit | neighbor_cost_bit);

  for(const auto next_four_info : next_four_info_list){
    const auto result_relaxed_four = AddRelaxedFour(next_four_info);
    const auto relaxed_four_id = result_relaxed_four.first;
    const auto is_generated = result_relaxed_four.second;

    if(!is_generated){
      continue;
    }

    additional_relaxed_four->emplace_back(relaxed_four_id);
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

    std::vector<BoardPosition> stone_position_list;
    pattern.GetStonePosition(&stone_position_list);

    next_four_info_list->emplace_back(
      next_gain, 
      next_cost, 
      GetBoardMove(stone_position_list[0]),
      GetBoardMove(stone_position_list[1]),
      GetBoardMove(stone_position_list[2])
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
    const bool is_feasible = move_feasible_relaxed_four_id_list_.find(open_rest) != move_feasible_relaxed_four_id_list_.end();

    if(!is_feasible){
      continue;
    }

    std::array<BoardPosition, 2> four_position;
    pattern.GetFourPosition(&four_position);

    std::vector<BoardPosition> stone_position_list;
    pattern.GetStonePosition(&stone_position_list);

    {
      const auto next_gain = GetBoardMove(four_position[0]);
      const auto next_cost = GetBoardMove(four_position[1]);

      next_four_info_list->emplace_back(
        next_gain,
        next_cost,
        open_rest,
        GetBoardMove(stone_position_list[0]),
        GetBoardMove(stone_position_list[1])
      );
    }
    {
      const auto next_gain = GetBoardMove(four_position[1]);
      const auto next_cost = GetBoardMove(four_position[0]);

      next_four_info_list->emplace_back(
        next_gain,
        next_cost,
        open_rest,
        GetBoardMove(stone_position_list[0]),
        GetBoardMove(stone_position_list[1])
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
      const auto &relaxed_four = GetRelaxedFour(additional_four_id);
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
  size_t feasible_four_count = 0;

  for(const auto move : GetAllInBoardMove()){
    const auto find_it = move_feasible_relaxed_four_id_list_.find(move);

    if(find_it == move_feasible_relaxed_four_id_list_.end()){
      continue;
    }

    const auto& relaxed_four_id_set_ptr = find_it->second;
    feasible_four_count += relaxed_four_id_set_ptr->size();
  }

  return feasible_four_count;
}

inline RelaxedFour& FourSpaceSearch::GetRelaxedFour(const RelaxedFourID relaxed_four_id)
{
  assert(relaxed_four_id < relaxed_four_list_.size());
  return *(relaxed_four_list_[relaxed_four_id]);
}

inline const RelaxedFour& FourSpaceSearch::GetRelaxedFour(const RelaxedFourID relaxed_four_id) const
{
  assert(relaxed_four_id < relaxed_four_list_.size());
  return *(relaxed_four_list_[relaxed_four_id]);
}

inline void FourSpaceSearch::AddFeasibleRelaxedFourID(const RelaxedFourID relaxed_four_id)
{
  const auto& relaxed_four = GetRelaxedFour(relaxed_four_id);
  const auto gain_position = relaxed_four.GetGainPosition();
  const auto& find_it = move_feasible_relaxed_four_id_list_.find(gain_position);

  if(find_it == move_feasible_relaxed_four_id_list_.end()){
    const auto& insert_result = 
    move_feasible_relaxed_four_id_list_.insert(
      make_pair(gain_position, std::make_unique<std::set<RelaxedFourID>>())
    );
    assert(insert_result.second);
    
    auto& relaxed_fourid_set_ptr = insert_result.first->second;
    relaxed_fourid_set_ptr->insert(relaxed_four_id);
  }else{
    auto& relaxed_fourid_set_ptr = find_it->second;
    relaxed_fourid_set_ptr->insert(relaxed_four_id);
  }
}

inline const size_t FourSpaceSearch::GetMaxRelaxedFourLength() const
{
  return four_space_manager_.GetMaxRelaxedFourLength();
}
}   // namespace realcore


#endif
