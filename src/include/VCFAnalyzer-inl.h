#ifndef VCF_ANALYZER_INL_H
#define VCF_ANALYZER_INL_H

#include "VCFAnalyzer.h"

namespace realcore{

inline constexpr bool IsVCFProved(const VCFSearchDepth value)
{
  return (kVCFProvedLB <= value && value <= kVCFProvedUB);
}

inline constexpr bool IsVCFDisproved(const VCFSearchDepth value)
{
  return (value == kVCFStrongDisproved);
}

inline constexpr bool IsVCFWeakDisproved(const VCFSearchDepth value)
{
  return (kVCFWeakDisprovedLB <= value && value <= kVCFWeakDisprovedUB);;
}

inline constexpr VCFSearchDepth GetVCFDepth(const VCFSearchDepth value)
{
  assert(IsVCFProved(value) || IsVCFWeakDisproved(value));

  if(IsVCFProved(value)){
    // value = kVCFProvedUB - (depth - 1) <=> depth = kVCFProvedUB - value + 1
    return kVCFProvedUB - value + 1;
  }else{
    // value = kVCFWeakDisprovedUB - (depth - 1) <=> depth = kVCFWeakDisprovedUB - value + 1
    return kVCFWeakDisprovedUB - value + 1;
  }
}

inline constexpr VCFSearchValue GetVCFProvedSearchValue(const VCFSearchDepth depth)
{
  assert(1 <= depth && depth <= static_cast<VCFSearchDepth>(kInBoardMoveNum));
  // value = kVCFProvedUB - (depth - 1)
  return kVCFProvedUB - (depth - 1);
}

inline constexpr VCFSearchValue GetVCFWeakDisprovedSearchValue(const VCFSearchDepth depth)
{
  assert(1 <= depth && depth <= static_cast<VCFSearchDepth>(kInBoardMoveNum));
  // value = kVCFWeakDisprovedUB - (depth - 1)
  return kVCFWeakDisprovedUB - (depth - 1);
}

template<PlayerTurn P>
VCFSearchValue VCFAnalyzer::SolveOR(const VCFSearch &vcf_search, VCFResult * const vcf_result)
{
  assert(vcf_result != nullptr);

  search_manager_.AddNode();

  if(search_manager_.IsTerminate()){
    return kVCFWeakDisprovedUB;
  }

  // 置換表をチェック
  const auto hash_value = CalcHashValue(board_move_sequence_);
  VCFSearchValue table_value = 0;
  const bool is_registered = vcf_table_->find(hash_value, bit_board_, &table_value);

  if(is_registered){
    if(IsVCFProved(table_value) || IsVCFDisproved(table_value)){
      return table_value;
    }

    const auto value_depth = GetVCFDepth(table_value);
    
    if(value_depth >= vcf_search.remain_depth){
      return table_value;
    }
  }

  // 候補手生成
  std::vector<MovePair> candidate_move;
  GetCandidateMoveOR<P>(vcf_search, &candidate_move);

  if(!is_registered){
    // 初回訪問時のみ終端チェックを行う
    for(auto &move_pair : candidate_move){
      if(IsTerminateMove(move_pair)){
        // 終端
        constexpr VCFSearchDepth depth = 1;
        constexpr VCFSearchValue search_value = GetVCFProvedSearchValue(depth);
        vcf_table_->Upsert(hash_value, bit_board_, search_value);

        vcf_result->best_response = move_pair.first;

        return search_value;
      }
    }
  }

  if(vcf_search.remain_depth == 1){
    // 残り深さ１で終端していなければ弱意の不詰
    constexpr VCFSearchDepth depth = 1;
    constexpr auto search_value = GetVCFWeakDisprovedSearchValue(depth);
    vcf_table_->Upsert(hash_value, bit_board_, search_value);
    return search_value;
  }

  // 展開
  VCFSearch child_vcf_search = vcf_search;
  child_vcf_search.remain_depth--;

  VCFSearchValue or_node_value = kVCFStrongDisproved;
  bool is_search_all_candidate = vcf_search.detect_dual_solution;   // 余詰探索用に全候補手を展開するかのフラグ

  for(const auto move_pair : candidate_move){
    const auto four_move = move_pair.first;
    const auto guard_move = move_pair.second;

    if(bit_board_.IsForbiddenMove<P>(four_move)){
      continue;
    }

    MakeMove(four_move);

    search_manager_.AddNode();
    VCFSearchValue and_node_value = kVCFProvedUB;

    if(IsTerminateMove(guard_move)){
      and_node_value = kVCFStrongDisproved;     // AND nodeが終端 -> 強意の不詰
    }else{
      MakeMove(guard_move);
      and_node_value = SolveOR<P>(child_vcf_search, vcf_result);
      UndoMove();
    }
  
    UndoMove();

    or_node_value = std::max(or_node_value, and_node_value);
    
    if(!is_search_all_candidate && IsVCFProved(or_node_value)){
      MoveList best_response;

      best_response += four_move;
      best_response += guard_move;
      best_response += vcf_result->best_response;
      vcf_result->best_response = best_response;

      break;
    }
  }

  const VCFSearchValue search_value = GetSearchValue(or_node_value);
  vcf_table_->Upsert(hash_value, bit_board_, search_value);
  return search_value;
}

template<PlayerTurn P>
void VCFAnalyzer::GetCandidateMoveOR(const VCFSearch &vlm_search, std::vector<MovePair> * const candidate_move) const
{
  assert(candidate_move != nullptr);
  assert(candidate_move->empty());

  MovePosition guard_move;
  
  if(IsOpponentFour(&guard_move)){
    // 相手に四がある
    MovePosition opponent_guard_move;

    if(bit_board_.IsFourMove<P>(guard_move, &opponent_guard_move)){
      candidate_move->emplace_back(guard_move, opponent_guard_move);
    }

    return;
  }

  // 四を生成する
  EnumerateFourMoves<P>(candidate_move);

  // todo move ordering
}

template<PlayerTurn P>
const bool VCFAnalyzer::DetectDualSolutionOR(MoveTree * const proof_tree, MoveList * const best_response, MoveTree * const dual_solution_tree)
{
  return false;
}

template<PlayerTurn P>
const bool VCFAnalyzer::GetProofTreeOR(MoveTree * const proof_tree, const bool generate_full_tree)
{
  return false;
}

inline const VCFSearchValue VCFAnalyzer::GetSearchValue(const VCFSearchValue child_search_value) const
{
  if(IsVCFDisproved(child_search_value)){
    return kVCFStrongDisproved;
  }

  return child_search_value - 1;
}

inline SearchManager& VCFAnalyzer::GetSearchManager()
{
  return search_manager_;
}

inline const SearchManager& VCFAnalyzer::GetSearchManager() const
{
  return search_manager_;
}

inline const bool VCFAnalyzer::IsRootNode() const
{
  return search_sequence_.empty();
}

}   // namespace realcore

#endif  // VCF_ANALYZER_INL_H