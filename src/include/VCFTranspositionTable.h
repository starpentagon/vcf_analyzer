//! @file
//! @brief VCF analyzerの置換表定義
//! @author Koichi NABETANI
//! @date 2017/07/28
#ifndef VCF_TRANSPOSITION_TABLE_H
#define VCF_TRANSPOSITION_TABLE_H

#include <array>
#include <vector>

#include "HashTable.h"
#include "VCFParameter.h"

namespace realcore{

typedef std::int16_t VCFSearchValue;   //!< 探索結果を表す値

}   // namespace realcore

#endif  // VCF_TRANSPOSITION_TABLE_H