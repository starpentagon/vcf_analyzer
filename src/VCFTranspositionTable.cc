#include "BitBoard.h"
#include "VCFTranspositionTable.h"
#include "VCFAnalyzer.h"

using namespace std;
using namespace realcore;

VCFTable::VCFTable(const size_t table_space, const bool lock_control)
: hash_table_(table_space, lock_control)
{
}

void VCFTable::Upsert(const HashValue hash_value, const BitBoard &bit_board, const VCFSearchValue search_value)
{
  VCFTableData table_data(hash_value, search_value);

#if kVCFTableUseExactBoardInfo
  bit_board.GetBoardStateBit(&table_data.board);
#endif

  VCFTableData in_table_data;
  const bool is_conflict = hash_table_.IsConflict(hash_value, &in_table_data);

  if(IsVCFProved(search_value)){
    // 詰むデータの登録
    // 登録先にすでに詰むデータが記録されている場合は別テーブルに登録する
    if(is_conflict && IsVCFProved(in_table_data.search_value)){
      proved_data_list_.emplace_back(table_data);
    }else{
      hash_table_.Upsert(hash_value, table_data);
    }
  }else{
    // 詰まないデータの登録
    // 登録先にすでに詰むデータが記録されている場合は登録を行わない
    if(is_conflict && IsVCFProved(in_table_data.search_value)){
      return;
    }

    hash_table_.Upsert(hash_value, table_data);
  }
}

const bool VCFTable::find(const HashValue hash_value, const BitBoard &bit_board, VCFSearchValue * const search_value) const
{
  VCFTableData table_data;
  auto find_result = hash_table_.IsConflict(hash_value, &table_data);

  if(!find_result){
    // hash値に対応する要素が未登録
    return false;
  }
  
  if(table_data.hash_value != hash_value){
    if(IsVCFProved(table_data.search_value)){
      // 別のHash値の詰むデータが登録されているため別テーブルに記録されている可能性がある
      find_result = false;

      for(const auto &proved_data : proved_data_list_){
        if(proved_data.hash_value == hash_value){
          table_data = proved_data;
          find_result = true;
          break;
        }
      }

      if(!find_result){
        return false;
      }
    }else{
      // 別のHash値のデータが登録済で詰むデータではない
      return false;
    }
  }
  
#if kVCFTableUseExactBoardInfo
  // 盤面が完全一致するかチェック
  array<StateBit, 8> board_info;
  bit_board.GetBoardStateBit(&board_info);

  for(size_t i=0; i<8; i++){
    if(board_info[i] != table_data.board[i]){
      return false;
    }
  }
#endif

  *search_value = table_data.search_value;
  return true;
}

void VCFTable::Initialize(){
  proved_data_list_.clear();
  hash_table_.Initialize();
}

std::string VCFTable::GetDefinitionInfo() const
{
  stringstream ss;

  ss << "VCF Transposition Info:" << endl;
  ss << "  Data type: " << (kVCFTableUseExactBoardInfo == 1 ? "Exact Board(" : "Hash Board(") << sizeof(VCFTableData) << " bytes)" << endl;
  ss << "  Table size: " << hash_table_.size() << endl;

  const auto space = round(10 * hash_table_.space()) / 10;
  ss << "  Table space: " << space << " MB" << endl;

  return ss.str();
}
