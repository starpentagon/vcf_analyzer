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
  const bool is_regiestered = RegisterOpenRestKeyFourSpace(open_rest_list_key, four_space_id);

  if(!is_regiestered){
    // すでに登録済のため抜ける
    return;
  }

  added_four_space_list->emplace_back(std::make_pair(open_rest_list_key, four_space_id));

  // open_rest_list_keyに依存する開残路キーのFourSpaceリストを更新する
  const auto& child_rest_key_set = open_rest_dependency_.GetChildSet(open_rest_list_key);
  
  if(child_rest_key_set.empty()){
    return;
  }

  std::vector<FourSpaceID> four_space_id_list{four_space_id};
  
  MoveBitSet rest_move_bit;
  GetOpenRestBit(open_rest_list_key, &rest_move_bit);
  
  for(const auto child_rest_key : child_rest_key_set){
    MoveBitSet child_rest_move_bit;
    GetOpenRestBit(child_rest_key, &child_rest_move_bit);
    
    MovePosition additional_move = GetAdditionalMove(rest_move_bit, child_rest_move_bit);
  
    std::vector<FourSpaceID> next_four_space_id_list;
    const auto& registered_four_space_id_list = GetFourSpaceIDList(additional_move);
    GeneratePuttableFourSpace<P>(four_space_id_list, registered_four_space_id_list, &next_four_space_id_list);

    for(const auto next_four_space : next_four_space_id_list){
      AddOpenRestListFourSpace<P>(child_rest_key, next_four_space, added_four_space_list);
    }
  }
}

template<PlayerTurn P>
void FourSpaceManager::GeneratePuttableFourSpace(const std::vector<FourSpaceID> &four_space_id_list_1, const std::vector<FourSpaceID> &four_space_id_list_2, std::vector<FourSpaceID> * const puttable_four_space_id_list)
{
  assert(puttable_four_space_id_list != nullptr);
  assert(puttable_four_space_id_list->empty());

  static constexpr PositionState S = GetPlayerStone(P);
  static constexpr PositionState T = GetPlayerStone(GetOpponentTurn(P));

  std::set<FourSpaceID> four_space_id_set;

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
        bit_board_.SetState<S>(four_space.GetGainBit());
        bit_board_.SetState<T>(four_space.GetCostBit());

        const bool is_five = bit_board_.IsFiveStones<P>();

        bit_board_.SetState<kOpenPosition>(four_space.GetGainBit() | four_space.GetCostBit());

        if(is_five){
          continue;
        }

        four_space_id = RegisterFourSpace(four_space);
      }

      four_space_id_set.insert(four_space_id);
    }
  }

  puttable_four_space_id_list->reserve(four_space_id_set.size());

  for(const auto four_space_id : four_space_id_set){
    puttable_four_space_id_list->emplace_back(four_space_id);
  }
}

template<PlayerTurn P>
void FourSpaceManager::EnumeratePuttableFourSpace(const OpenRestList &open_rest_list, std::vector<FourSpaceID> * const puttable_four_space_id_list)
{
  assert(puttable_four_space_id_list != nullptr);
  assert(puttable_four_space_id_list->empty());

  const auto& rest_move_list = open_rest_list.GetOpenRestMoveList();
  const auto rest_size = rest_move_list.size();
  assert(rest_size <= 3);

  if(rest_size == 0){
    return;
  }

  const auto rest_list_key = open_rest_list.GetOpenRestKey();
  const auto find_it = open_rest_key_four_space_id_.find(rest_list_key);

  if(find_it != open_rest_key_four_space_id_.end()){
    // 生成済みの場合
    *puttable_four_space_id_list = find_it->second;
    return;
  }

  // 新たに生成が必要になるのは獲得/損失空間の組合せを考慮する場合のみ
  assert(rest_size >= 2);   
  std::vector<MovePosition> sub_rest_move_list(rest_move_list);
  
  const auto move = sub_rest_move_list.back();
  sub_rest_move_list.pop_back();

  std::vector<FourSpaceID> sub_four_space_list;
  OpenRestList sub_open_rest(sub_rest_move_list);

  EnumeratePuttableFourSpace<P>(sub_open_rest, &sub_four_space_list);

  const auto& move_four_space_list = GetFourSpaceIDList(move);

  std::vector<FourSpaceID> generated_four_space_id_list;
  GeneratePuttableFourSpace<P>(move_four_space_list, sub_four_space_list, &generated_four_space_id_list);

  open_rest_key_four_space_id_.insert(
    make_pair(rest_list_key, generated_four_space_id_list)
  );

  open_rest_dependency_.Add(rest_list_key);

  *puttable_four_space_id_list = generated_four_space_id_list;
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

inline void FourSpaceManager::AddOpenRestListKey(const OpenRestListKey open_rest_list_key)
{
  open_rest_dependency_.Add(open_rest_list_key);
}

}   // namespace realcore

#endif    // FOUR_SPACE_MANAGER_H
