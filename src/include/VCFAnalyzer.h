//! @file
//! @brief VCF解図を行うクラス
//! @author Koichi NABETANI
//! @date 2017/07/28
#ifndef VCF_ANALYZER_H
#define VCF_ANALYZER_H

#include <cstdint>
#include <climits>

#include "Board.h"
#include "VCFTranspositionTable.h"

namespace realcore
{

typedef std::int16_t VCFSearchDepth;   //!< 探索深さ

constexpr VCFSearchDepth kVCFStrongDisproved = -32317;    //!< 強意の不詰(= VCFSearchDepth + 1)
constexpr VCFSearchDepth kVCFWeakDisprovedLB = kVCFStrongDisproved + 1;    //!< 弱意の不詰(最小値)
constexpr VCFSearchDepth kVCFWeakDisprovedUB = kVCFWeakDisprovedLB + (static_cast<VCFSearchDepth>(kInBoardMoveNum) - 1);    //!< 弱意の不詰(最大値)
constexpr VCFSearchDepth kVCFProvedLB = kVCFWeakDisprovedUB + 1;    //!< 詰む場合(最小値)
constexpr VCFSearchDepth kVCFProvedUB = kVCFProvedLB + (static_cast<VCFSearchDepth>(kInBoardMoveNum) - 1);    //!< 詰む場合(最大値)

//! @brief 詰む場合の値かどうかを判定する
constexpr bool IsVCFProved(const VCFSearchDepth value);

//! @brief 強意の不詰かどうかを判定する
constexpr bool IsVCFDisproved(const VCFSearchDepth value);

//! @brief 弱意の不詰かどうかを判定する
constexpr bool IsVCFWeakDisproved(const VCFSearchDepth value);

//! @brief 終端局面までの手数を返す
//! @pre valueは詰む値 or 弱意の不詰であること
constexpr VCFSearchDepth GetVCFDepth(const VCFSearchDepth value);

//! @brief 終端局面までの手数から探索結果値を返す
constexpr VLMSearchValue GetVLMProvedSearchValue(const VLMSearchDepth depth);

//! @brief 弱意の不詰の探索結果値を返す
constexpr VLMSearchValue GetVLMWeakDisprovedSearchValue(const VLMSearchDepth depth);


}   // namespace realcore

#endif  // VCF_ANALYZER_H