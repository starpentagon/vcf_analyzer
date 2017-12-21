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

  static std::vector<MovePosition> answer_gain{
    kMoveND, kMoveLD, kMoveOD, kMoveMD, kMoveME, kMoveMF, kMoveLF, kMoveOG, kMoveNF, kMoveKF,
    kMoveEO, kMoveAK, kMoveAL, kMoveDA, kMoveBA, kMoveBD, kMoveCC, kMoveDD, kMoveCE, kMoveCD,
    kMoveCG, kMoveDF, kMoveGC, kMoveEC, kMoveAG, kMoveAH, kMoveFC, kMoveEB, kMoveGD, kMoveGE,
    kMoveHC, kMoveFE, kMoveNE, kMoveLG, kMoveLA, kMoveLE, kMoveJE, kMoveGF, kMoveIH, kMoveHG,
    kMoveHI, kMoveEL, kMoveBI, kMoveFH, kMoveEH, kMoveDI, kMoveCI, kMoveCK, kMoveDL, kMoveFI,
    kMoveEM, kMoveFN, kMoveEN, kMoveHN, kMoveGK, kMoveKK, kMoveIK, kMoveHL, kMoveNH, kMoveMI,
    kMoveKI, kMoveOI, kMoveOJ, kMoveOM, kMoveMM, kMoveKM, kMoveNM, kMoveKL, kMoveML, kMoveMO,
    kMoveLN, kMoveKO, kMoveLO
  };

  static std::vector<MovePosition> answer_cost{
    kMoveLB, kMoveKE, kMoveOB, kMoveKD, kMoveMB, kMoveMG, kMoveKG, kMoveOF, kMoveKC, kMoveJF,
    kMoveAO, kMoveCM, kMoveAJ, kMoveFA, kMoveAA, kMoveBC, kMoveDB, kMoveFF, kMoveDE, kMoveCB,
    kMoveCF, kMoveBH, kMoveFD, kMoveFB, kMoveBF, kMoveAF, kMoveDC, kMoveED, kMoveHE, kMoveGB,
    kMoveIC, kMoveJA, kMoveNC, kMoveKH, kMoveNA, kMoveLC, kMoveID, kMoveGG, kMoveJG, kMoveJI,
    kMoveHJ, kMoveFK, kMoveDK, kMoveEG, kMoveGH, kMoveBK, kMoveEI, kMoveCH, kMoveCL, kMoveIL,
    kMoveBJ, kMoveGO, kMoveEK, kMoveGN, kMoveFL, kMoveII, kMoveJK, kMoveHO, kMoveNG, kMoveLJ,
    kMoveLH, kMoveNI, kMoveOH, kMoveOL, kMoveNN, kMoveNJ, kMoveLM, kMoveKJ, kMoveMJ, kMoveMN,
    kMoveJL, kMoveKN, kMoveIO, kMoveNO
  };

  static MoveBitSet answer_gain_bit, answer_cost_bit;

  if(answer_gain_bit.none()){
    for(const auto move : answer_gain){
      answer_gain_bit.set(move);
    }

    for(const auto move : answer_cost){
      answer_cost_bit.set(move);
    }
  }

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
  // todo delete
  bool is_answer = (answer_gain_bit & four_space.GetGainBit()) == four_space.GetGainBit();
  is_answer &= (answer_cost_bit & four_space.GetCostBit()) == four_space.GetCostBit();
/*
  if(!is_answer){
    return;
  }
*/

  if(is_answer){
    std::cerr << "answer: " << MoveString(move) << ", " << four_space.GetGainBit().count() << ", ";

    MoveList gain_list, cost_list, move_list;
    GetMoveList(four_space.GetGainBit(), &gain_list);
    GetMoveList(four_space.GetCostBit(), &cost_list);

    for(size_t i=0, size=gain_list.size(); i<size; i++){
      move_list += gain_list[i];
      move_list += cost_list[i];
    }

    std::cerr << gain_list.str() << ", " << cost_list.str() << ", " << move_list.str() << std::endl;
  }

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
  if(++check_count % 100000 == 0){
    std::cerr << "add four space check / count = " << 1.0 * check_count / count << "\t" << check_count << " / " << count << std::endl;
    std::cerr << "\tR-four count: " << GetRelaxedFourCount() << std::endl;
    std::cerr << "\tR-four length: " << GetMaxRelaxedFourLength() << std::endl;

    std::cerr << "\tR-four count on move" << std::endl;
    ShowBoardRelaxedFourCount();

    std::cerr << "\tFourSpace count on move" << std::endl;
    ShowBoardGainCostSpaceCount();
  }

  // 位置moveを残路に持つ緩和四ノビごとに獲得/損失空間を追加できるかチェックする
  std::map<OpenRestListKey, std::vector<FourSpace>> additional_four_space;
  UpdateRestListPuttableFourSpace<P>(move, four_space, &additional_four_space);
  UpdateAdditionalPuttableFourSpace<P>(move, four_space, additional_four_space);

  // 位置moveの直線近傍から新たに緩和四ノビを作れるかチェックする
  std::vector<RelaxedFourID> relaxed_four_id_list;
  GenerateRelaxedFour<P>(move, four_space, &relaxed_four_id_list);

  for(const auto relaxed_four_id : relaxed_four_id_list){
    auto& relaxed_four = GetRelaxedFour(relaxed_four_id);
    const auto& open_rest_list = relaxed_four.GetOpenRestList();

    std::vector<FourSpace> rest_four_space_list;
    EnumeratePuttableFourSpace<P>(open_rest_list, &rest_four_space_list);

    ExpandRelaxedFour<P>(relaxed_four_id, rest_four_space_list);
  }
}

template<PlayerTurn P>
void FourSpaceSearch::ExpandRelaxedFour(const RelaxedFourID relaxed_four_id, const std::vector<FourSpace> &rest_four_space_list)
{
  auto& relaxed_four = GetRelaxedFour(relaxed_four_id);
  const auto gain_position = relaxed_four.GetGainPosition();
  const auto cost_position = relaxed_four.GetCostPosition();

  for(const auto& rest_four_space : rest_four_space_list){
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

      std::map<OpenRestListKey, std::vector<FourSpace>> additional_four_space;
      UpdateRestListPuttableFourSpace<P>(gain_position, child_four_space, &additional_four_space);

      // todo delete --
/*      MoveList term_gain, term_cost;
      GetMoveList(child_four_space.GetGainBit(), &term_gain);
      GetMoveList(child_four_space.GetCostBit(), &term_cost);
*/      //std::cerr << "terminate: " << term_gain.str() << ", " << term_cost.str() << std::endl;
      // -- todo delete

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

    AddFourSpace<P>(gain_position, child_four_space);
  }
}

template<PlayerTurn P>
void FourSpaceSearch::UpdateAdditionalPuttableFourSpace(const MovePosition move, const FourSpace &four_space, const std::map<OpenRestListKey, std::vector<FourSpace>> &additional_four_space)
{
  for(const auto& additional : additional_four_space){
    const auto rest_key = additional.first;
    const auto &additional_four_space_list = additional.second;

    // rest_keyと同一の開残路を持つ緩和四ノビIDを取得
    const auto find_it = rest_list_relaxed_four_list_.find(rest_key);

    if(find_it == rest_list_relaxed_four_list_.end()){
      continue;
    }

    const auto& relaxed_four_id_list_ptr = find_it->second;
    const std::vector<RelaxedFourID> relaxed_four_id_list = *relaxed_four_id_list_ptr;  // rest_list_relaxed_four_list_が拡張される可能性があるためコピーを作成

    for(const auto relaxed_four_id : relaxed_four_id_list){
      ExpandRelaxedFour<P>(relaxed_four_id, additional_four_space_list);
    }
  }
}

template<PlayerTurn P>
void FourSpaceSearch::UpdateRestListPuttableFourSpace(const OpenRestListKey rest_key, const FourSpace &four_space, std::map<OpenRestListKey, std::vector<FourSpace>> * const additional_four_space)
{
  assert(additional_four_space != nullptr);

  if(IsRegisteredFourSpace(rest_key, four_space)){
    return;
  }

  static constexpr bool kSkipRegisterCheck = false;
  const bool is_added = AddRestListFourSpace<P>(rest_key, four_space, kSkipRegisterCheck);

  if(is_added){
    (*additional_four_space)[rest_key].emplace_back(four_space);
  }

  const auto find_it = rest_key_tree_.find(rest_key);

  if(find_it == rest_key_tree_.end()){
    return;
  }

  const auto& child_rest_key_set = find_it->second;

  std::vector<FourSpace> four_space_list{four_space};

  MoveBitSet rest_move_bit;
  GetOpenRestBit(rest_key, &rest_move_bit);

  for(const auto child_rest_key : child_rest_key_set){
    MoveBitSet child_rest_move_bit;
    GetOpenRestBit(child_rest_key, &child_rest_move_bit);
    
    MovePosition additional_move = GetAdditionalMove(rest_move_bit, child_rest_move_bit);
  
    std::vector<FourSpace> next_four_space_list;
    const auto& registered_four_space_list = GetFourSpaceList(additional_move);
    GeneratePuttableFourSpace(four_space_list, registered_four_space_list, &next_four_space_list);

    for(const auto next_four_space : next_four_space_list){
      UpdateRestListPuttableFourSpace<P>(child_rest_key, next_four_space, additional_four_space);
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

  SetMoveBit<S>(neighbor_gain_bit);
  SetMoveBit<T>(neighbor_cost_bit);

  LineNeighborhood line_neighborhood(gain_position, kOpenStateNeighborhoodSize, *this);

  // すでに生成済みかチェックする
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

template<PlayerTurn P>
void FourSpaceSearch::EnumeratePuttableFourSpace(const OpenRestList &open_rest_list, std::vector<FourSpace> * const puttable_four_space_list)
{
  assert(puttable_four_space_list != nullptr);
  assert(puttable_four_space_list->empty());

  const auto& rest_move_list = open_rest_list.GetOpenRestMoveList();
  const auto rest_size = rest_move_list.size();
  assert(rest_size <= 3);

  if(rest_size == 0){
    return;
  }

  const auto rest_list_key = open_rest_list.GetOpenRestKey();
  const auto find_it = rest_list_puttable_four_space_.find(rest_list_key);

  if(find_it != rest_list_puttable_four_space_.end()){
    // 生成済みの場合
    *puttable_four_space_list = *(find_it->second);
    return;
  }

  // 新たに生成が必要になるのは獲得/損失空間の組合せを考慮する場合のみ
  assert(rest_size >= 2);   
  std::vector<MovePosition> sub_rest_move_list(rest_move_list);
  
  const auto move = sub_rest_move_list.back();
  sub_rest_move_list.pop_back();

  std::vector<FourSpace> move_four_space_list, sub_four_space_list, four_space_list;
  EnumeratePuttableFourSpace<P>(sub_rest_move_list, &sub_four_space_list);

  move_four_space_list = GetFourSpaceList(move);

  GeneratePuttableFourSpace(move_four_space_list, sub_four_space_list, &four_space_list);

  static constexpr bool kSkipRegisterCheck = false;
  AddRestListFourSpace<P>(rest_list_key, four_space_list, kSkipRegisterCheck, puttable_four_space_list);
}

template<PlayerTurn P>
const bool FourSpaceSearch::AddRestListFourSpace(const OpenRestListKey rest_key, const FourSpace &four_space, const bool is_register_check)
{
  if(is_register_check && IsRegisteredFourSpace(rest_key, four_space)){
    return false;
  }

  // 五が生じる獲得/損失空間は生成しない
  // @note BitBoardオブジェクトを作るよりSetStateで設置/クリアをしたほうが数%高速
  static constexpr PositionState S = GetPlayerStone(P);
  static constexpr PositionState T = GetPlayerStone(GetOpponentTurn(P));

  SetMoveBit<S>(four_space.GetGainBit());
  SetMoveBit<T>(four_space.GetCostBit());

  const bool is_five = IsFiveStones<kBlackTurn>() || IsFiveStones<kWhiteTurn>();

  SetMoveBit<kOpenPosition>(four_space.GetGainBit());
  SetMoveBit<kOpenPosition>(four_space.GetCostBit());

  if(is_five){
    return false;
  }

  const auto& find_it = rest_list_puttable_four_space_.find(rest_key);

  if(find_it == rest_list_puttable_four_space_.end()){
    const auto& insert_result =
    rest_list_puttable_four_space_.insert(make_pair(rest_key, std::make_unique<std::vector<FourSpace>>()));
    assert(insert_result.second);

    auto& four_space_vector_ptr = insert_result.first->second;
    four_space_vector_ptr->emplace_back(four_space);
  }else{
    auto& four_space_vector_ptr = find_it->second;
    four_space_vector_ptr->emplace_back(four_space);
  }

  UpdateRestListKeyTree(rest_key);

  return true;
}

template<PlayerTurn P>
void FourSpaceSearch::AddRestListFourSpace(const OpenRestListKey rest_key, const std::vector<FourSpace> &four_space_list, const bool is_register_check, std::vector<FourSpace> * const added_four_space_list)
{
  assert(added_four_space_list != nullptr);
  assert(added_four_space_list->empty());

  for(const auto& four_space : four_space_list){
    const bool is_added = AddRestListFourSpace<P>(rest_key, four_space, is_register_check);

    if(is_added){
      added_four_space_list->emplace_back(four_space);
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

inline const std::vector<FourSpace>& FourSpaceSearch::GetFourSpaceList(const MovePosition move) const
{
  const auto find_it = rest_list_puttable_four_space_.find(move);
  static std::vector<FourSpace> empty_list;

  if(find_it == rest_list_puttable_four_space_.end()){
    return empty_list;
  }else{
    return *(find_it->second);
  }
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

}   // namespace realcore


#endif
