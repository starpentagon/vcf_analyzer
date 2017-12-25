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

const bool FourSpaceManager::RegisterOpenRestKeyFourSpace(const OpenRestListKey open_rest_list_key, const FourSpaceID four_space_id)
{
  const auto find_it = open_rest_key_four_space_id_.find(open_rest_list_key);

  if(find_it == open_rest_key_four_space_id_.end()){
    const auto insert_result = open_rest_key_four_space_id_.insert(
      make_pair(open_rest_list_key, vector<FourSpaceID>())
    );

    open_rest_dependency_.Add(open_rest_list_key);

    assert(insert_result.second);
    
    auto& four_space_id_list = insert_result.first->second;
    four_space_id_list.emplace_back(four_space_id);

    return true;
  }

  auto& four_space_id_list = find_it->second;

  const auto four_id_it = find(four_space_id_list.begin(), four_space_id_list.end(), four_space_id);

  if(four_id_it != four_space_id_list.end()){
    // 登録済
    return false;
  }

  four_space_id_list.emplace_back(four_space_id);
  
  return true;
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
