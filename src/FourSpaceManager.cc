#include "FourSpaceManager.h"

using namespace std;
using namespace realcore;

FourSpaceManager::FourSpaceManager(const BitBoard &bit_board)
: bit_board_(bit_board), puttable_check_count_(0), puttable_count_(0)
{
  // kInvalidFourSpaceIDに対応する要素を追加
  four_space_list_.emplace_back();
}

const FourSpaceID FourSpaceManager::GetFourSpaceID(const FourSpace &four_space) const
{
  const auto four_space_hash = four_space.CalcHashValue();
  auto range = four_space_hash_table_.equal_range(four_space_hash);

  for(auto it=range.first; it!=range.second; ++it){
    const auto four_space_id = it->second;
    auto four_space_id_index = four_space_id;

    if(IsInfeasibleFourSpace(four_space_id_index)){
      static constexpr FourSpaceID kInfeasibleFourSpaceFlag = 1ULL << 63;
      four_space_id_index ^= kInfeasibleFourSpaceFlag;
    }

    const auto& registered_four_space = GetFourSpace(four_space_id_index);
    
    if(registered_four_space == four_space){
      return four_space_id;
    }
  }

  return kInvalidFourSpaceID;
}

const size_t FourSpaceManager::GetMaxRelaxedFourLength() const
{
  size_t max_length = 0;

  for(const auto& four_space : four_space_list_){
    const size_t length = four_space.GetGainBit().count();

    max_length = std::max(length, max_length);
  }

  return max_length;
}

const tuple<MovePosition, OpenRestListKey, bool> FourSpaceManager::GetParentOpenRestListKey(const OpenRestListKey open_rest_list_key) const
{
  if(IsSingleRestMove(open_rest_list_key)){
    const auto find_it = open_rest_key_puttable_four_space_id_.find(open_rest_list_key);
    const bool is_member = find_it != open_rest_key_puttable_four_space_id_.end();
    return make_tuple(kNullMove, open_rest_list_key, is_member);
  }

  vector<MovePosition> rest_move_list;
  GetOpenRestMoveList(open_rest_list_key, &rest_move_list);

  for(const auto parent_move : rest_move_list){
    vector<MovePosition> parent_rest_move_list(rest_move_list);
    const auto erase_it = find(parent_rest_move_list.begin(), parent_rest_move_list.end(), parent_move);
    parent_rest_move_list.erase(erase_it);

    OpenRestList parent_rest_list(parent_rest_move_list);
    const auto parent_key = parent_rest_list.GetOpenRestKey();

    const auto find_it = open_rest_key_puttable_four_space_id_.find(parent_key);

    if(find_it != open_rest_key_puttable_four_space_id_.end()){
      return make_tuple(parent_move, parent_key, true);
    }
  }

  // 親が未登録の場合
  vector<MovePosition> parent_rest_move_list(rest_move_list);
  const auto parent_move = parent_rest_move_list.back();
  parent_rest_move_list.pop_back();

  OpenRestList parent_rest_list(parent_rest_move_list);
  const auto parent_key = parent_rest_list.GetOpenRestKey();
  return make_tuple(parent_move, parent_key, false);
}

void FourSpaceManager::OutputPuttableSummary() const
{
  const auto size = open_rest_key_puttable_four_space_id_.size();
  cerr << "Puttable key size: " << size << endl;

  size_t puttable_count = 0;

  for(const auto& element : open_rest_key_puttable_four_space_id_){
    puttable_count += element.second.size();
  }

  cerr << "Puttable size: " << puttable_count << endl;
  cerr << "Puttable average size: " << (1.0 * puttable_count / size) << endl;
  cerr << endl;
  cerr << "Puttable check: " << puttable_check_count_ << endl;
  cerr << "Puttable count: " << puttable_count_ << "(" << (100.0 * puttable_count_ / puttable_check_count_) << "%)" << endl;
  cerr << endl;
}

void FourSpaceManager::OutputFeasibleSummary() const
{
  const auto size = open_rest_key_feasible_four_space_id_.size();
  cerr << "Feasible key size: " << size << endl;

  size_t feasible_count = 0;

  for(const auto& element : open_rest_key_feasible_four_space_id_){
    feasible_count += element.second.size();
  }

  cerr << "Feasible size: " << feasible_count << endl;
  cerr << "Feasible average size: " << (1.0 * feasible_count / size) << endl;
}
