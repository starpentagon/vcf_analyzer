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

    FourSpace four_space(four_attack, four_guard);

    AddRelaxedFour(four_attack, four_guard, null_rest_list);

    AddFourSpace<P>(four_attack, four_space);
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
void FourSpaceSearch::AddFourSpace(const MovePosition move, const FourSpace &four_space)
{
  // todo delete
  static size_t count = 0;
  count++;

  if(IsRegisteredFourSpace(move, four_space)){
    // すでに登録済みの獲得/損失空間がある場合は終了
    static size_t skipped_add_four_space = 0;

    // todo delete
    skipped_add_four_space++;
    //std::cerr << "skipped_add_four_space rate: " << 1.0 * skipped_add_four_space / count << std::endl;
    return;
  }

  // todo delete
  static size_t check_count = 0;

  // todo delete
  if(++check_count % 10000 == 0){
    std::cerr << "add four space check / count = " << 1.0 * check_count / count << "\t" << check_count << " / " << count << std::endl;
    std::cerr << "\tR-four count: " << GetRelaxedFourCount() << std::endl;
    //std::cerr << "\tR-four length: " << GetMaxRelaxedFourLength() << std::endl;

    std::cerr << "\tR-four count on move" << std::endl;

    for(const auto move : GetAllInBoardMove()){
      Cordinate x, y;
      GetMoveCordinate(move, &x, &y);

      std::cerr << move_gain_list_[move].size() << ",";

      if(x == 15){
        std::cerr << std::endl;
      }
    }

    std::cerr << "\tFourSpace count on move" << std::endl;
    
    for(const auto move : GetAllInBoardMove()){
      Cordinate x, y;
      size_t four_space_count = 0;
      GetMoveCordinate(move, &x, &y);

      four_space_count = GetFourSpaceList(move).size();

      std::cerr << four_space_count << ",";

      if(x == 15){
        std::cerr << std::endl;
      }
    }
  }

  move_four_space_list_[move].emplace_back(four_space);

  // 位置moveを残路に持つ緩和四ノビごとに獲得/損失空間を追加できるかチェックする
  UpdateAdditionalPuttableFourSpace(move, four_space);

  // 位置moveの直線近傍から緩和四ノビを作れるかチェックする
  GenerateRelaxedFour<P>(move, four_space);
}

template<PlayerTurn P>
void FourSpaceSearch::GenerateRelaxedFour(const MovePosition gain_position, const FourSpace &four_space)
{
  static constexpr PlayerTurn Q = GetOpponentTurn(P);
  static constexpr PositionState S = GetPlayerStone(P);
  static constexpr PositionState T = GetPlayerStone(Q);

  // todo delete
  static size_t count = 0;

  if(++count % 10000 == 0){
    std::cerr << "GenerateRelaxedFour: " << count << std::endl;
  }

  // 追加する獲得/損失空間と直線近傍のANDパターンを生成
  MoveBitSet neighborhood_bit;
  GetLineNeighborhoodBit(gain_position, kOpenStateNeighborhoodSize, &neighborhood_bit);

  const auto &neighbor_gain_bit = four_space.GetNeighborhoodGainBit(neighborhood_bit);
  const auto &neighbor_cost_bit = four_space.GetNeighborhoodCostBit(neighborhood_bit);

  SetMoveBit<S>(neighbor_gain_bit);
  SetMoveBit<T>(neighbor_cost_bit);

  LineNeighborhood line_neighborhood(gain_position, kOpenStateNeighborhoodSize, *this);

  // すでにチェック済みか調べる
  LocalBitBoard local_bit_board;
  line_neighborhood.GetLocalBitBoard(&local_bit_board);

  if(IsRegisteredLocalBitBoard(gain_position, local_bit_board)){
    SetMoveBit<kOpenPosition>(neighbor_gain_bit);
    SetMoveBit<kOpenPosition>(neighbor_cost_bit);

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
  
  SetMoveBit<kOpenPosition>(neighbor_gain_bit);
  SetMoveBit<kOpenPosition>(neighbor_cost_bit);

  // 開残路リストごとに同時設置可能な獲得/損失空間を取得する
  std::map<RestListKey, std::vector<FourSpace>> rest_key_four_space_list;
  std::set<RestListKey> checked_rest_key_set;
  
  for(const auto next_four_info : next_four_info_list){
    const MovePosition rest_1 = std::get<2>(next_four_info);
    const MovePosition rest_2 = std::get<3>(next_four_info);
    const MovePosition rest_3 = std::get<4>(next_four_info);

    std::vector<MovePosition> rest_list;
    rest_list.reserve(3);

    for(const auto rest_move : {rest_1, rest_2, rest_3}){
      if(!move_gain_list_[rest_move].empty()){
        rest_list.emplace_back(rest_move);
      }
    }

    const auto rest_key = GetOpenRestKey(rest_list);

    if(checked_rest_key_set.find(rest_key) != checked_rest_key_set.end()){
      continue;
    }

    checked_rest_key_set.insert(rest_key);

    // 獲得/損失空間の追加による新たな同時設置可能な獲得/損失空間を求める
    std::vector<MovePosition> brother_rest_list;
    GetRestPosition(gain_position, rest_key, &brother_rest_list);
    std::vector<FourSpace> puttable_four_space_list;
    EnumeratePuttableFourSpace(four_space, brother_rest_list, &puttable_four_space_list);

    // 登録済みかチェックする

    for(const auto& puttable_four_space : puttable_four_space_list){
      if(IsRegisteredFourSpace(rest_key, puttable_four_space)){
        rest_list_puttable_four_space_[rest_key].emplace_back(puttable_four_space);
        // continue;    // todo skipできるのか要検討
      }

      rest_key_four_space_list[rest_key].emplace_back(puttable_four_space);
    }
  }

  for(const auto next_four_info : next_four_info_list){
    const MovePosition next_gain = std::get<0>(next_four_info);
    const MovePosition next_cost = std::get<1>(next_four_info);

    const MovePosition rest_1 = std::get<2>(next_four_info);
    const MovePosition rest_2 = std::get<3>(next_four_info);
    const MovePosition rest_3 = std::get<4>(next_four_info);

    std::vector<MovePosition> rest_list;
    rest_list.reserve(3);

    for(const auto rest_move : {rest_1, rest_2, rest_3}){
      if(!move_gain_list_[rest_move].empty()){
        rest_list.emplace_back(rest_move);
      }
    }

    const auto rest_key = GetOpenRestKey(rest_list);
    const auto& four_space_list = rest_key_four_space_list[rest_key];

    if(four_space_list.empty()){
      continue;
    }

    // 重複チェック
    // todo AddRelaxedFourと処理が被っているの集約する
    RelaxedFour relaxed_four(next_gain, next_cost, rest_list);
    const auto key = relaxed_four.GetKey();

    const auto find_it = transposition_table_.find(key);
    
    if(find_it != transposition_table_.end()){
      continue;
    }

    // todo 五ができている場合はスキップ
    for(auto next_four_space : four_space_list){
      if(next_four_space.IsConflict(next_gain, next_cost)){
        continue;
      }

      // 獲得/損失空間を着手した場合に緩和四ノビが成立するかチェック
      SetMoveBit<S>(next_four_space.GetGainBit());
      SetMoveBit<T>(next_four_space.GetCostBit());

      MovePosition guard_move;
      const bool is_four = IsFourMove<P>(next_gain, &guard_move);
      const bool is_forbidden = IsForbiddenMove<P>(next_gain);
      bool opponent_guard_failed = false;

      if(is_four){
        // 四ノリがある場合にノリ返せるかチェック
        SetState<S>(next_gain);
        SetState<T>(guard_move);

        MovePosition opponent_guard;
        const bool is_opponent_four = IsFourMoveOnBoard<Q>(guard_move, &opponent_guard);

        if(is_opponent_four){
          opponent_guard_failed = !IsFourMove<P>(opponent_guard, NULL);
        }

        SetState<kOpenPosition>(next_gain);
        SetState<kOpenPosition>(guard_move);
      }
      
      SetMoveBit<kOpenPosition>(next_four_space.GetGainBit());
      SetMoveBit<kOpenPosition>(next_four_space.GetCostBit());

      if(!is_four || is_forbidden || guard_move != next_cost || opponent_guard_failed){
        static size_t four_check_skip_count = 0;

        if(++four_check_skip_count % 1000 == 0){
          std::cerr << "four_check_skip_count: " << four_check_skip_count << std::endl;
        }

        continue;
      }

      AddRelaxedFour(next_gain, next_cost, rest_list);
      next_four_space.Add(next_gain, next_cost);
      AddFourSpace<P>(next_gain, next_four_space);
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

    if(move_gain_list_[open_rest].empty()){
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
  return relaxed_four_list_.size() - 1;   // -1: 先頭のダミーデータを除く
}

template<PositionState S>
inline void FourSpaceSearch::SetState(const MovePosition move)
{
  BitBoard::SetState<S>(move);
}

inline const std::vector<RelaxedFourID>& FourSpaceSearch::GetGainRelaxedFourIDList(const MovePosition gain_position) const
{
  return move_gain_list_[gain_position];
}

inline const RelaxedFour& FourSpaceSearch::GetRelaxedFour(const RelaxedFourID relaxed_four_id) const
{
  assert(relaxed_four_id < relaxed_four_list_.size());
  return relaxed_four_list_[relaxed_four_id];
}

inline void FourSpaceSearch::GetRestRelaxedFourIDList(const MovePosition move, std::vector<RelaxedFourID> * const rest_relaxed_four_id_list) const
{
  assert(rest_relaxed_four_id_list != nullptr);
  *rest_relaxed_four_id_list = move_open_rest_list_[move];
}

inline const std::vector<FourSpace>& FourSpaceSearch::GetFourSpaceList(const MovePosition move) const
{
  return move_four_space_list_[move];
}

template<PositionState S>
inline void FourSpaceSearch::SetMoveBit(const MoveBitSet &move_bit)
{
  MoveList move_list;
  GetMoveList(move_bit, &move_list);

  for(const auto move : move_list){
    SetState<S>(move);
  }
}

}   // namespace realcore


#endif
