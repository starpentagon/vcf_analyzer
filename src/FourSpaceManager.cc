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
    const auto& registered_four_space = GetFourSpace(four_space_id);
    
    if(registered_four_space == four_space){
      return four_space_id;
    }
  }

  return kInvalidFourSpaceID;
}

const FourSpaceID FourSpaceManager::RegisterFourSpace(const FourSpace &four_space)
{
  assert(GetFourSpaceID(four_space) == kInvalidFourSpaceID);

  four_space_list_.emplace_back(four_space);
  const size_t four_space_id = four_space_list_.size() - 1;

  const auto four_space_hash = four_space.CalcHashValue();
  four_space_hash_table_.emplace(four_space_hash, four_space_id);

  return four_space_id;
}

void FourSpaceManager::GeneratePuttableFourSpace(const std::vector<FourSpaceID> &four_space_id_list_1, const std::vector<FourSpaceID> &four_space_id_list_2, std::vector<FourSpaceID> * const puttable_four_space_id_list)
{
  assert(puttable_four_space_id_list != nullptr);
  assert(puttable_four_space_id_list->empty());

  set<FourSpaceID> puttable_four_space_id_set;

  puttable_check_count_ += four_space_id_list_1.size() * four_space_id_list_2.size();

  for(const auto four_space_id_1 : four_space_id_list_1){
    const auto &four_space_1 = GetFourSpace(four_space_id_1);

    for(const auto four_space_id_2 : four_space_id_list_2){
      const auto &four_space_2 = GetFourSpace(four_space_id_2);
    
      if(!four_space_1.IsPuttable(four_space_2)){
        continue;
      }

      FourSpace four_space(four_space_1);
      four_space.Add(four_space_2);

      FourSpaceID four_space_id = GetFourSpaceID(four_space);

      if(four_space_id == kInvalidFourSpaceID){
        four_space_id = RegisterFourSpace(four_space);
      }

      puttable_four_space_id_set.insert(four_space_id);
    }
  }

  puttable_four_space_id_list->reserve(puttable_four_space_id_set.size());
  puttable_count_ += puttable_four_space_id_set.size();

  for(const auto four_space_id : puttable_four_space_id_set){
    puttable_four_space_id_list->emplace_back(four_space_id);
  }
}

void FourSpaceManager::GeneratePuttableFourSpace(const OpenRestListKey open_rest_list_key, std::vector<FourSpaceID> * const puttable_four_space_id_list)
{
  assert(puttable_four_space_id_list != nullptr);
  assert(puttable_four_space_id_list->empty());

  const auto find_it = open_rest_key_puttable_four_space_id_.find(open_rest_list_key);

  if(find_it != open_rest_key_puttable_four_space_id_.end()){
    *puttable_four_space_id_list = find_it->second;
    return;
  }

  // 未生成の場合
  const auto parent_info = GetParentOpenRestListKey(open_rest_list_key);
  const auto rest_move = get<0>(parent_info);
  const auto sub_rest_list_key = get<1>(parent_info);

  vector<FourSpaceID> sub_four_space_id_list;
  GeneratePuttableFourSpace(sub_rest_list_key, &sub_four_space_id_list);
  
  const auto& move_four_space_id_list = GetPuttableFourSpace(rest_move);

  GeneratePuttableFourSpace(move_four_space_id_list, sub_four_space_id_list, puttable_four_space_id_list);
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
  if(open_rest_list_key <= kMoveNum){
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

void FourSpaceManager::IsPuttableConsistent()
{
  // 設置可能なFourSpaceが親から子が作れているかチェックする
  for(const auto &element : open_rest_key_puttable_four_space_id_){
    const auto open_rest_list_key = element.first;

    std::vector<MovePosition> rest_move_list;
    GetOpenRestMoveList(open_rest_list_key, &rest_move_list);
    const auto rest_size = rest_move_list.size();

    if(rest_size <= 1){
      continue;
    }

    const auto& puttable_four_space_id_list = element.second;
    bool is_checked = false;

    for(size_t i=0; i<rest_size; i++){
      const auto rest_move = rest_move_list[i];
      const auto parent_key = realcore::GetParentOpenRestListKey(rest_move, open_rest_list_key);

      const auto parent_it = open_rest_key_puttable_four_space_id_.find(parent_key);

      if(parent_it == open_rest_key_puttable_four_space_id_.end()){
        continue;
      }

      is_checked = true;

      const auto& check_puttable_list_1 = GetPuttableFourSpace(rest_move);
      const auto& check_puttable_list_2 = GetPuttableFourSpace(parent_key);

      std::vector<FourSpaceID> check_puttable_list;
      GeneratePuttableFourSpace(check_puttable_list_1, check_puttable_list_2, &check_puttable_list);

      if(puttable_four_space_id_list.size() != check_puttable_list.size()){
        std::cerr << "[error]Puttable inconsistent: size is not same at key = " << open_rest_list_key << std::endl;
        continue;
      }

      for(const auto four_space_id : check_puttable_list){
        const auto check_it = find(puttable_four_space_id_list.begin(), puttable_four_space_id_list.end(), four_space_id);

        if(check_it == puttable_four_space_id_list.end()){
          std::cerr << "[error]Puttable inconsistent: four space id(" << four_space_id << ") is not contained" << std::endl;
          break;
        }
      }
    }

    if(!is_checked){
      std::cerr << "[error]Puttable inconsistent: there is no valid child key" << std::endl;
    }
  }
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
