//! @file
//! @brief VCF Analyzerのパラメタを定義する
//! @author Koichi NABETANI
//! @date 2017/07/28
#ifndef VCF_PARAMETER_H
#define VCF_PARAMETER_H

#include <cstdint>

#include "OpenState.h"

namespace realcore{

//! @brief 置換表に盤面情報をすべて用いるかどうかのフラグ
//! @note (pros)正確に同一局面を判定できる
//! @note (cons)テーブル要素サイズが16byte->80byteに増え同一容量で保持できる要素数が約1/5になる
#define kVCFTableUseExactBoardInfo 1

//! @brief 置換表のデフォルトサイズ(1GB)
static constexpr std::size_t kDefaultVCFTableSpace = 1024;

//! @brief デフォルトでは置換表のロックをしない
static constexpr bool kDefaultVCFTableLockFree = false;

//! @brief デフォルトでは置換表でInterruption ExceptionをCatchしない
static constexpr bool kDefaultVCTTableNoInterruptionException = false;

}   // namespace realcore

#endif    // VCF_PARAMETER_H