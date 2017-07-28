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

// 盤面をHash値で持つ場合のデータ構造
typedef struct structVCFHashData{
  structVCFHashData()
  : hash_value(0), logic_counter(0), search_value(0)
  {
  }

  structVCFHashData(const HashValue hash, const VCFSearchValue search)
  : hash_value(hash), search_value(search)
  {
  }
  
  HashValue hash_value;               // Hash値
  TableLogicCounter logic_counter;    // 論理カウンタ
  VCFSearchValue search_value;        // 探索結果
}VCFHashData;

// 盤面情報をすべて持つ場合のデータ構造
typedef struct structVCFExactData{
  structVCFExactData()
  : hash_value(0), board{{0}}, logic_counter(0), search_value(0)
  {
  }

  structVCFExactData(const HashValue hash, const VCFSearchValue search)
  : hash_value(hash), board{{0}}, logic_counter(0), search_value(search)
  {
  }

  HashValue hash_value;               // Hash値
  std::array<StateBit,8> board;                  // 盤面情報
  TableLogicCounter logic_counter;    // 論理カウンタ
  VCFSearchValue search_value;        // 探索結果
}VCFExactData;

#if kVCFTableUseExactBoardInfo
typedef VCFExactData VCFTableData;
#else
typedef VCFHashData VCFTableData;
#endif

// 前方宣言
class BitBoard;

class VCFTable
{
public:
  //! @brief 置換表を確保する
  //! @param table_mb_size HashTableのサイズ(MB)
  VCFTable(const size_t table_space, const bool lock_control);

  //! @brief 置換表へのupsertを行う
  //! @param hash_value 局面のハッシュ値
  //! @param bit_board 現局面のBitBoard
  //! @param search_value 登録する探索結果
  void Upsert(const HashValue hash_value, const BitBoard &bit_board, const VCFSearchValue search_value);

  //! @brief 置換表の検索を行う
  //! @param search_value 探索結果の格納先
  //! @retval true 置換表にデータがある
  const bool find(const HashValue hash_value, const BitBoard &bit_board, VCFSearchValue * const search_value) const;

  //! @brief 置換表の定義情報を取得する
  std::string GetDefinitionInfo() const;

  //! @brief 初期化を行う
  void Initialize();

private:
  HashTable<VCFTableData> hash_table_;
  std::vector<VCFTableData> proved_data_list_;    // 証明木を取得するため詰むデータを記録する
};
}   // namespace realcore

#endif  // VCF_TRANSPOSITION_TABLE_H