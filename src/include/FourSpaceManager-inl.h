#ifndef FOUR_SPACE_MANAGER_INL_H
#define FOUR_SPACE_MANAGER_INL_H

#include "FourSpaceManager.h"

namespace realcore{

template<PlayerTurn P>
void FourSpaceManager::AddFourSpace(const MovePosition gain_move, const MovePosition cost_move, const FourSpace &four_space, std::vector<RestKeyFourSpace> * const added_four_space_list)
{
  assert(added_four_space_list != nullptr);
  assert(added_four_space_list->empty());

  FourSpaceID four_space_id = GetFourSpaceID(four_space);

  if(four_space_id == kInvalidFourSpaceID){
    // 未登録の場合は登録する
    four_space_id = RegisterFourSpace(four_space);
  }
  
  AddOpenRestListFourSpace<P>(gain_move, four_space_id, added_four_space_list);
}

template<PlayerTurn P>
void FourSpaceManager::AddOpenRestListFourSpace(const OpenRestListKey open_rest_list_key, const FourSpaceID four_space_id, std::vector<RestKeyFourSpace> * const added_four_space_list)
{

}

inline const FourSpace& FourSpaceManager::GetFourSpace(const FourSpaceID four_space_id) const
{
  assert(four_space_id < four_space_list_.size());
  return four_space_list_[four_space_id];
}

inline const std::vector<FourSpaceID>& FourSpaceManager::GetFourSpaceIDList(const OpenRestListKey open_rest_list_key) const
{
  const auto find_it = open_rest_key_four_space_id_.find(open_rest_list_key);
  static std::vector<FourSpaceID> empty_list;

  if(find_it == open_rest_key_four_space_id_.end()){
    return empty_list;
  }else{
    return find_it->second;
  }
}

}   // namespace realcore

#endif    // FOUR_SPACE_MANAGER_H
