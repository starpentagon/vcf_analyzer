#include <set>  // todo delete

#include "FourSpaceSearch.h"

using namespace std;

namespace realcore{

FourSpaceSearch::FourSpaceSearch(const BitBoard &bit_board)
: BitBoard(bit_board), four_space_manager_(bit_board)
{
  vector<MovePosition> null_rest_list;
  vector<FourSpace> null_four_space_list;
  
  // kInvalidFourID用にダミーデータを設定する
  relaxed_four_list_.emplace_back(
    make_unique<RelaxedFour>(kNullMove, kNullMove, null_rest_list)
  );
}

const pair<RelaxedFourID, bool> FourSpaceSearch::AddRelaxedFour(const NextRelaxedFourInfo &next_relaxed_four)
{
  const MovePosition next_gain = get<0>(next_relaxed_four);
  const MovePosition next_cost = get<1>(next_relaxed_four);

  OpenRestList open_rest_list;
  GetRestList(next_relaxed_four, &open_rest_list);

  RelaxedFour relaxed_four(next_gain, next_cost, open_rest_list.GetOpenRestMoveList());
  return AddRelaxedFour(relaxed_four);
}

const pair<RelaxedFourID, bool> FourSpaceSearch::AddRelaxedFour(const RelaxedFour &relaxed_four)
{
  const auto key = relaxed_four.GetKey();
  const auto find_it = transposition_table_.find(key);

  if(find_it != transposition_table_.end()){
    return make_pair(find_it->second, false);
  }

  relaxed_four_list_.emplace_back(make_unique<RelaxedFour>(relaxed_four));

  const RelaxedFourID four_id = relaxed_four_list_.size() - 1;
  transposition_table_.insert(make_pair(key, four_id));
  
  const auto& open_rest_list = relaxed_four.GetOpenRestList();
  const auto rest_key = open_rest_list.GetOpenRestKey();

  const auto& four_id_find_it = rest_list_relaxed_four_list_.find(rest_key);

  if(four_id_find_it == rest_list_relaxed_four_list_.end()){
    const auto& insert_result =
    rest_list_relaxed_four_list_.insert(make_pair(rest_key, make_unique<vector<RelaxedFourID>>()));
    assert(insert_result.second);

    auto& relaxed_fourid_vector_ptr = insert_result.first->second;
    relaxed_fourid_vector_ptr->emplace_back(four_id);
  }else{
    auto& relaxed_fourid_vector_ptr = four_id_find_it->second;
    relaxed_fourid_vector_ptr->emplace_back(four_id);
  }

  four_space_manager_.AddOpenRestListKey(rest_key);
  return make_pair(four_id, true);
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
    const auto find_it = move_feasible_relaxed_four_id_list_.find(check_move);

    if(find_it == move_feasible_relaxed_four_id_list_.end()){
      continue;
    }

    const auto& relaxed_id_set_ptr = find_it->second;

    for(const auto relaxed_four_id : *relaxed_id_set_ptr){
      restable_four_id_list->emplace_back(relaxed_four_id);
    }

    open_rest_count++;
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
    const auto find_it = move_feasible_relaxed_four_id_list_.find(check_move);

    if(find_it == move_feasible_relaxed_four_id_list_.end()){
      continue;
    }

    const auto& relaxed_id_set_ptr = find_it->second;

    for(const auto relaxed_four_id : *relaxed_id_set_ptr){
      restable_four_id_list->emplace_back(relaxed_four_id);
    }

    open_rest_count++;
  }

  return open_rest_count;
}

const bool FourSpaceSearch::IsRegisteredLocalBitBoard(const MovePosition move, const LocalBitBoard &local_bitboard) const
{
  const auto& local_bitboard_list = move_local_bitboard_list_[move];

  for(const auto& registered_bitboard : local_bitboard_list){
    if(registered_bitboard == local_bitboard){
      return true;
    }
  }

  return false;
}

void FourSpaceSearch::ShowBoardRelaxedFourCount() const
{
  for(const auto move : GetAllInBoardMove()){
    Cordinate x, y;
    GetMoveCordinate(move, &x, &y);

    const auto find_it = move_feasible_relaxed_four_id_list_.find(move);
    size_t count = 0;
    
    if(find_it != move_feasible_relaxed_four_id_list_.end()){
      const auto& relaxed_fourid_set_ptr = find_it->second;
      count = relaxed_fourid_set_ptr->size();
    }

    cerr << count << ",";

    if(x == 15){
      cerr << endl;
    }
  }
}

void FourSpaceSearch::ShowBoardFourSpaceCount() const
{
  for(const auto move : GetAllInBoardMove()){
    const auto four_space_count = four_space_manager_.GetFourSpaceCount(move);
    cerr << four_space_count << ",";

    Cordinate x, y;
    GetMoveCordinate(move, &x, &y);

    if(x == 15){
      cerr << endl;
    }
  }  
}

const OpenRestListKey FourSpaceSearch::GetRestList(const NextRelaxedFourInfo &next_four_info, OpenRestList * const open_rest_list) const
{
  assert(open_rest_list != nullptr);
  assert(open_rest_list->empty());

  const MovePosition rest_1 = std::get<2>(next_four_info);
  const MovePosition rest_2 = std::get<3>(next_four_info);
  const MovePosition rest_3 = std::get<4>(next_four_info);

  for(const auto rest_move : {rest_1, rest_2, rest_3}){
    const bool is_feasible = move_feasible_relaxed_four_id_list_.find(rest_move) != move_feasible_relaxed_four_id_list_.end();

    if(is_feasible){
      open_rest_list->Add(rest_move);
    }
  }

  const auto rest_key = open_rest_list->GetOpenRestKey();
  return rest_key;
}

void FourSpaceSearch::GetGainMoveRelaxedFourIDList(MoveRelaxedFourIDList * const move_relaxed_four_id_list) const
{
  assert(move_relaxed_four_id_list != nullptr);
  
  for(const auto move : GetAllInBoardMove()){
    const auto find_it = move_feasible_relaxed_four_id_list_.find(move);

    if(find_it == move_feasible_relaxed_four_id_list_.end()){
      continue;
    }

    const auto& relaxed_four_id_set_ptr = find_it->second;

    for(const auto relaxed_four_id : *relaxed_four_id_set_ptr){
      (*move_relaxed_four_id_list)[move].emplace_back(relaxed_four_id);
    }
  }
}

void FourSpaceSearch::GetCostMoveRelaxedFourIDList(MoveRelaxedFourIDList * const move_relaxed_four_id_list) const
{
  assert(move_relaxed_four_id_list != nullptr);
  
  for(const auto move : GetAllInBoardMove()){
    const auto find_it = move_feasible_relaxed_four_id_list_.find(move);

    if(find_it == move_feasible_relaxed_four_id_list_.end()){
      continue;
    }

    const auto& relaxed_four_id_set_ptr = find_it->second;

    for(const auto relaxed_four_id : *relaxed_four_id_set_ptr){
      const auto& relaxed_four = GetRelaxedFour(relaxed_four_id);
      const auto cost = relaxed_four.GetCostPosition();

      (*move_relaxed_four_id_list)[cost].emplace_back(relaxed_four_id);
    }
  }
}

void FourSpaceSearch::TenYearsFeverCheck(const MovePosition gain_move, const FourSpace &four_space) const
{
  // 十年フィーバー
  static MoveList board_move_list("hhgigjfgjjfjeeifhkijlijhllefcjejhmdgmcdjmkjdbblkbefmblgmdnimibjcnbjmaemhaibgambnbocncahbcojndokbeankganljoabkaacmaadoafoochaoeonokoo");
  static BitBoard bit_board(board_move_list);

  if(bit_board != *this){
    return;
  }
  
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

  bool is_answer = (answer_gain_bit & four_space.GetGainBit()) == four_space.GetGainBit();
  is_answer &= (answer_cost_bit & four_space.GetCostBit()) == four_space.GetCostBit();

  if(is_answer){
    std::cerr << "answer: " << MoveString(gain_move) << ", " << four_space.GetGainBit().count() << ", ";

    MoveList gain_list, cost_list, move_list;
    GetMoveList(four_space.GetGainBit(), &gain_list);
    GetMoveList(four_space.GetCostBit(), &cost_list);

    for(size_t i=0, size=gain_list.size(); i<size; i++){
      move_list += gain_list[i];
      move_list += cost_list[i];
    }

    std::cerr << gain_list.str() << ", " << cost_list.str() << ", " << move_list.str() << std::endl;
  }
}

}   // namespace realcore
