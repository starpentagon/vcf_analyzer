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

}   // namespace realcore

#endif  // VCF_ANALYZER_INL_H