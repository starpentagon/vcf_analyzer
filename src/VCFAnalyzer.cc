#include "VCFAnalyzer.h"

using namespace std;

namespace realcore{
VCFAnalyzer::VCFAnalyzer(const MoveList &board_move_sequence)
: Board(board_move_sequence, board_move_sequence.IsBlackTurn() ? kUpdateVCFAnalyzerBlack : kUpdateVCFAnalyzerWhite), 
  search_manager_(kDefaultVCFTableNoInterruptionException)
{
  vcf_table_ = make_shared<VCFTable>(kDefaultVCFTableSpace, kDefaultVCFTableLockFree);
}

VCFAnalyzer::VCFAnalyzer(const MoveList &board_move_sequence, const shared_ptr<VCFTable> &vcf_table)
: Board(board_move_sequence, board_move_sequence.IsBlackTurn() ? kUpdateVCFAnalyzerBlack : kUpdateVCFAnalyzerWhite), 
  search_manager_(kCatchInterruptException), vcf_table_(vcf_table)
{
}

void VCFAnalyzer::Solve(const VCFSearch &vcf_search, VCFResult * const vcf_result)
{
  assert(vcf_result != nullptr);

  const bool is_black_turn = board_move_sequence_.IsBlackTurn();
  VCFSearch vcf_search_iterative = vcf_search;
  VCFSearchValue search_value = kVCFStrongDisproved;

  for(VCFSearchDepth depth=1; depth<=vcf_search.remain_depth; depth++){
    vcf_search_iterative.remain_depth = depth;

    search_value = SolveOR(is_black_turn, vcf_search_iterative, vcf_result);

    if(IsVCFProved(search_value) || IsVCFDisproved(search_value)){
      break;
    }

    if(search_manager_.IsTerminate()){
      break;
    }
  }

  // 探索結果
  vcf_result->solved = IsVCFProved(search_value);
  vcf_result->disproved = IsVCFDisproved(search_value);
  vcf_result->search_depth = vcf_search_iterative.remain_depth;

  if(search_manager_.IsTerminate() && vcf_result->search_depth >= 3){
    vcf_result->search_depth--;
  }

  if(vcf_result->solved){
    GetProofTree(&vcf_result->proof_tree);
  }

  if(vcf_result->solved && vcf_search.detect_dual_solution && vcf_result->search_depth >= 3){
    vcf_result->detect_dual_solution = DetectDualSolution(&vcf_result->proof_tree, &vcf_result->best_response, &vcf_result->dual_solution_tree);
  }
}

VCFSearchValue VCFAnalyzer::SolveOR(const bool is_black_turn, const VCFSearch &vcf_search, VCFResult * const vcf_result)
{
  if(is_black_turn){
    return SolveOR<kBlackTurn>(vcf_search, vcf_result);
  }else{
    return SolveOR<kWhiteTurn>(vcf_search, vcf_result);
  }
}

void VCFAnalyzer::MakeMove(const MovePosition move)
{
  search_sequence_ += move;
  Board::MakeMove(move);
}

void VCFAnalyzer::UndoMove()
{
  --search_sequence_;
  Board::UndoMove();
}

template<>
void VCFAnalyzer::MoveOrderingOR<kBlackTurn>(const VCFSearch &vcf_search, MoveBitSet * const candidate_move_bit, MoveList * const candidate_move) const
{
}

template<>
void VCFAnalyzer::MoveOrderingOR<kWhiteTurn>(const VCFSearch &vcf_search, MoveBitSet * const candidate_move_bit, MoveList * const candidate_move) const
{
}

const bool VCFAnalyzer::GetProofTree(MoveTree * const proof_tree)
{
  assert(proof_tree != nullptr);

  proof_tree->MoveRootNode();

  const bool is_black_turn = board_move_sequence_.IsBlackTurn();
  bool is_generated = false;

  if(is_black_turn){
    is_generated = GetProofTreeOR<kBlackTurn>(proof_tree, kGenerateFullTree);
  }else{
    is_generated = GetProofTreeOR<kWhiteTurn>(proof_tree, kGenerateFullTree);
  }

  if(!is_generated){
    proof_tree->clear();
  }

  return is_generated;
}

const bool VCFAnalyzer::DetectDualSolution(MoveTree * const proof_tree, MoveList * const best_response, MoveTree * const dual_solution_tree)
{
  assert(dual_solution_tree != nullptr);
  assert(dual_solution_tree->empty());
  assert(proof_tree != nullptr);
  assert(proof_tree->IsRootNode());

  dual_solution_tree->MoveRootNode();

  const bool is_black_turn = board_move_sequence_.IsBlackTurn();
  bool detect_dual_solution = false;

  if(is_black_turn){
    detect_dual_solution = DetectDualSolutionOR<kBlackTurn>(proof_tree, best_response, dual_solution_tree);
  }else{
    detect_dual_solution = DetectDualSolutionOR<kWhiteTurn>(proof_tree, best_response, dual_solution_tree);
  }

  if(!detect_dual_solution){
    dual_solution_tree->clear();
  }

  proof_tree->MoveRootNode();

  return detect_dual_solution;
}

void VCFAnalyzer::GetPreTerminateHash(MoveTree * const proof_tree, std::set<HashValue> * const pre_terminate_hash_set) const
{
  assert(proof_tree != nullptr);
  assert(pre_terminate_hash_set != nullptr);
  assert(pre_terminate_hash_set->empty());

  std::vector<MoveNodeIndex> leaf_index_list;

  proof_tree->GetLeafNodeList(&leaf_index_list);
  const bool is_black_turn = board_move_sequence_.IsBlackTurn();

  for(const auto leaf_index : leaf_index_list){
    proof_tree->MoveNode(leaf_index);
    proof_tree->MoveParent();

    MoveList move_list;
    proof_tree->GetMoveList(&move_list);

    HashValue hash_value = 0;

    // OR nodeのHash値を求める
    for(size_t i=0, size=move_list.size(); i<size; i+=2){
      hash_value = CalcHashValue(is_black_turn, move_list[i], hash_value);
    }

    pre_terminate_hash_set->insert(hash_value);
  }
}

const std::string VCFAnalyzer::GetSettingInfo() const
{
  stringstream ss;
  
  // Build mode
  ss << "Build:";

  #ifdef NDEBUG
    ss << "Release" << endl;
  #else
    ss << "Debug" << endl;
  #endif

  // 置換表の定義情報
  ss << vcf_table_->GetDefinitionInfo();
  return ss.str();
}
}   // namespace realcore
