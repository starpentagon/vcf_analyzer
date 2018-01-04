#include "FourSpaceManager.h"

using namespace std;
using namespace realcore;

FourSpaceManager::FourSpaceManager(const BitBoard &bit_board)
: bit_board_(bit_board)
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
  vector<MovePosition> rest_move_list;
  GetOpenRestMoveList(open_rest_list_key, &rest_move_list);
  assert(rest_move_list.size() >= 2);

  const auto rest_move = rest_move_list.back();
  rest_move_list.pop_back();

  OpenRestList sub_open_rest_list(rest_move_list);
  const auto sub_rest_list_key = sub_open_rest_list.GetOpenRestKey();

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
