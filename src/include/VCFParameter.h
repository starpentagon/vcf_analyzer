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
static constexpr bool kDefaultVCFTableNoInterruptionException = false;

// VLM Analyzer用(黒)の空点情報の更新
static constexpr uint64_t kUpdateFlagVCFAnalyzerBlack = 
  kUpdateFlagFourBlack;   // OR node(黒)での四生成
  
// VLM Analyzer用(白)の空点情報の更新
static constexpr uint64_t kUpdateFlagVCFAnalyzerWhite = 
  kUpdateFlagFourWhite;   // OR node(白)での四生成

constexpr UpdateOpenStateFlag kUpdateVCFAnalyzerBlack(kUpdateFlagVCFAnalyzerBlack);
constexpr UpdateOpenStateFlag kUpdateVCFAnalyzerWhite(kUpdateFlagVCFAnalyzerWhite);

}   // namespace realcore

#endif    // VCF_PARAMETER_H