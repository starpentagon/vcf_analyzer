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

  if(!is_registered){
    // 初回訪問時のみ終端チェックを行う
    MovePosition terminating_move;
    const bool is_terminate = TerminateCheck(&terminating_move);

    if(is_terminate){
      // 終端
      constexpr VCFSearchDepth depth = 1;
      constexpr VCFSearchValue search_value = GetVCFProvedSearchValue(depth);
      vcf_table_->Upsert(hash_value, bit_board_, search_value);
      return search_value;
    }
  }

  if(vcf_search.remain_depth == 1){
    // 残り深さ１で終端していなければ弱意の不詰
    constexpr VCFSearchDepth depth = 1;
    constexpr auto search_value = GetVCFWeakDisprovedSearchValue(depth);
    vcf_table_->Upsert(hash_value, bit_board_, search_value);
    return search_value;
  }

  // 候補手生成
  MoveList candidate_move;
  GetCandidateMoveOR<P>(vcf_search, &candidate_move);

  // 展開
  VCFSearch child_vcf_search = vcf_search;
  constexpr PlayerTurn Q = GetOpponentTurn(P);
  VCFSearchValue or_node_value = kVCFStrongDisproved;
  bool is_search_all_candidate = vcf_search.detect_dual_solution;   // 余詰探索用に全候補手を展開するかのフラグ

  // 多重反復深化
  const VCFSearchDepth max_child_depth = vcf_search.remain_depth - 1;
  const VCFSearchDepth min_child_depth = is_registered ? max_child_depth : 2;

  for(VCFSearchDepth child_depth=min_child_depth; child_depth<=max_child_depth; child_depth+=2){
    child_vcf_search.remain_depth = child_depth;

    for(const auto move : candidate_move){
      MakeMove(move);
      VCFSearchValue and_node_value = SolveAND<Q>(child_vcf_search, vcf_result);
      UndoMove();

      or_node_value = std::max(or_node_value, and_node_value);
      
      if(is_search_all_candidate && !IsRootNode() && IsVCFProved(or_node_value)){
        const auto vcf_depth = GetVCFDepth(or_node_value);

        if(vcf_depth < max_child_depth){
          // 残り深さ未満で解が見つかった -> 親ノードは弱防のため余詰探索を行わない
          is_search_all_candidate = false;
        }
      }
      
      if(!is_search_all_candidate && IsVCFProved(or_node_value)){
        child_depth += max_child_depth;   // child_depthのloopを抜けるため最大深さを超える値を設定する
        break;
      }
    }
  }

  const VCFSearchValue search_value = GetSearchValue(or_node_value);
  vcf_table_->Upsert(hash_value, bit_board_, search_value);
  return search_value;
}


}   // namespace realcore

#endif  // VCF_ANALYZER_INL_H