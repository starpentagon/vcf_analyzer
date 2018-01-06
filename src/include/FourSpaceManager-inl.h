#ifndef FOUR_SPACE_MANAGER_INL_H
#define FOUR_SPACE_MANAGER_INL_H

#include "FourSpaceManager.h"

namespace realcore{

template<PlayerTurn P>
const FourSpaceID FourSpaceManager::RegisterFourSpace(const FourSpace &four_space)
{
  assert(GetFourSpaceID(four_space) == kInvalidFourSpaceID);

  four_space_list_.emplace_back(four_space);
  size_t four_space_id = four_space_list_.size() - 1;
  static constexpr FourSpaceID kInfeasibleFourSpaceFlag = 1ULL << 63;

  if(IsFiveFourSpace<P>(four_space_id)){
    four_space_id |= kInfeasibleFourSpaceFlag;
  }

  const auto four_space_hash = four_space.CalcHashValue();
  four_space_hash_table_.emplace(four_space_hash, four_space_id);

  return four_space_id;
}

inline const bool FourSpaceManager::IsInfeasibleFourSpace(const FourSpaceID four_space_id) const
{
  static constexpr FourSpaceID kInfeasibleFourSpaceFlag = 1ULL << 63;
  return ((four_space_id & kInfeasibleFourSpaceFlag) != 0);
}

template<PlayerTurn P>
void FourSpaceManager::AddFourSpace(const MovePosition gain_move, const MovePosition cost_move, const FourSpace &four_space, std::vector<RestKeyFourSpace> * const added_four_space_list)
{
  assert(added_four_space_list != nullptr);
  assert(added_four_space_list->empty());

  FourSpaceID four_space_id = GetFourSpaceID(four_space);

  if(four_space_id == kInvalidFourSpaceID){
    // 未登録の場合は登録する
    four_space_id = RegisterFourSpace<P>(four_space);

    if(IsInfeasibleFourSpace(four_space_id)){
      return;
    }
  }

  AddOpenRestListFourSpace<P>(gain_move, four_space_id, added_four_space_list);
}

template<PlayerTurn P>
void FourSpaceManager::AddOpenRestListFourSpace(const MovePosition gain_move, const FourSpaceID four_space_id, std::vector<RestKeyFourSpace> * const added_four_space_list)
{
  const bool is_regiestered = RegisterOpenRestKeyFourSpace<P>(gain_move, four_space_id);

  if(!is_regiestered){
    // すでに登録済のため抜ける
    return;
  }

  added_four_space_list->emplace_back(std::make_pair(gain_move, four_space_id));

  // open_rest_list_keyに依存する開残路キーのFourSpaceリストを更新する
  std::set<OpenRestListKey> child_rest_key_set;
  open_rest_dependency_.GetAllDependentKeys(gain_move, &child_rest_key_set);
  
  if(child_rest_key_set.empty()){
    return;
  }

  // 更新が必要な開残路キーを求める
  std::vector<OpenRestListKey> update_child_key_list;
  update_child_key_list.reserve(child_rest_key_set.size());

  for(const auto rest_key : child_rest_key_set){
    const auto find_it = open_rest_key_puttable_four_space_id_.find(rest_key);
    
    if(find_it != open_rest_key_puttable_four_space_id_.end()){
      // すでに登録済の開残路キー
      update_child_key_list.emplace_back(rest_key);
    }
  }

  std::vector<FourSpaceID> four_space_id_list{four_space_id};
  
  for(const auto child_rest_key : update_child_key_list){
    const auto parent_key = realcore::GetParentOpenRestListKey(gain_move, child_rest_key);
    const auto parent_it = open_rest_key_puttable_four_space_id_.find(parent_key);
  
    if(parent_it == open_rest_key_puttable_four_space_id_.end()){
      CreateOpenRestKey<P>(parent_key);
    }

    const auto &child_four_space_id_list = GetPuttableFourSpace(parent_key);

    std::vector<FourSpaceID> puttable_four_space_id_list;
    GeneratePuttableFourSpace<P>(four_space_id_list, child_four_space_id_list, &puttable_four_space_id_list);

    for(const auto puttable_four_space_id : puttable_four_space_id_list){
      const bool is_registered = RegisterOpenRestKeyFourSpace<P>(child_rest_key, puttable_four_space_id);
      
      if(!is_registered){
        continue;
      }

      added_four_space_list->emplace_back(std::make_pair(child_rest_key, puttable_four_space_id));
      //std::cerr << "New: key: " << child_rest_key << " = " << GetOpenRestKeyString(child_rest_key) << ", id: " << puttable_four_space_id << std::endl;
    }
  }
}

template<PlayerTurn P>
void FourSpaceManager::CreateOpenRestKey(const OpenRestListKey open_rest_list_key)
{
  assert(open_rest_key_puttable_four_space_id_.find(open_rest_list_key) == open_rest_key_puttable_four_space_id_.end());

  std::vector<FourSpaceID> puttable_four_space_id_list;
  std::vector<FourSpaceID> feasible_four_space_id_list;

  if(!IsSingleRestMove(open_rest_list_key)){
    // 開残路が2つ以上の場合は組合せを生成する
    GeneratePuttableFourSpace<P>(open_rest_list_key, &puttable_four_space_id_list);
    GenerateFeasibleFourSpace<P>(puttable_four_space_id_list, &feasible_four_space_id_list);
  }

  open_rest_key_puttable_four_space_id_.insert(
    make_pair(open_rest_list_key, puttable_four_space_id_list)
  );

  open_rest_key_feasible_four_space_id_.insert(
    make_pair(open_rest_list_key, feasible_four_space_id_list)
  );

  open_rest_dependency_.Add(open_rest_list_key);

  // 親が存在しているかチェックし、存在しない場合は生成する
  const auto parent_info = GetParentOpenRestListKey(open_rest_list_key);
  const auto is_registered = std::get<2>(parent_info);
  
  if(!is_registered){
    const auto parent_key = std::get<1>(parent_info);
    CreateOpenRestKey<P>(parent_key);
  }
}

template<PlayerTurn P>
const bool FourSpaceManager::RegisterOpenRestKeyFourSpace(const OpenRestListKey open_rest_list_key, const FourSpaceID four_space_id)
{
  const auto find_it = open_rest_key_puttable_four_space_id_.find(open_rest_list_key);
  assert(GetFourSpace(four_space_id).IsPuttable());

  if(find_it == open_rest_key_puttable_four_space_id_.end()){
    CreateOpenRestKey<P>(open_rest_list_key);
  }

  auto& puttable_four_space_id_list = open_rest_key_puttable_four_space_id_.find(open_rest_list_key)->second;
  auto& feasible_four_space_id_list = open_rest_key_feasible_four_space_id_.find(open_rest_list_key)->second;

  const bool is_puttable_inserted = InsertList(puttable_four_space_id_list, four_space_id);

  if(!is_puttable_inserted){
    // 登録済
    return false;
  }

  if(IsFeasibleFourSpace<P>(four_space_id)){
    feasible_four_space_id_list.emplace_back(four_space_id);
  }else{
    // 実現可能ではない
    return false;
  }

  return true;
}

template<PlayerTurn P>
void FourSpaceManager::RegisterOpenRestKeyFourSpace(const OpenRestListKey open_rest_list_key, const std::vector<FourSpaceID> &four_space_id_list)
{
  for(const auto four_space_id : four_space_id_list){
    RegisterOpenRestKeyFourSpace<P>(four_space_id);
  }
}

inline const std::vector<FourSpaceID>& FourSpaceManager::GetPuttableFourSpace(const OpenRestListKey open_rest_list_key) const
{
  const auto find_it = open_rest_key_puttable_four_space_id_.find(open_rest_list_key);
  assert(find_it != open_rest_key_puttable_four_space_id_.end());

  return find_it->second;
}

template<PlayerTurn P>
void FourSpaceManager::GeneratePuttableFourSpace(const std::vector<FourSpaceID> &four_space_id_list_1, const std::vector<FourSpaceID> &four_space_id_list_2, std::vector<FourSpaceID> * const puttable_four_space_id_list)
{
  assert(puttable_four_space_id_list != nullptr);
  assert(puttable_four_space_id_list->empty());

  std::set<FourSpaceID> puttable_four_space_id_set;

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
        four_space_id = RegisterFourSpace<P>(four_space);
      }

      if(IsInfeasibleFourSpace(four_space_id)){
        continue;
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

template<PlayerTurn P>
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
  const auto rest_move = std::get<0>(parent_info);
  const auto sub_rest_list_key = std::get<1>(parent_info);

  std::vector<FourSpaceID> sub_four_space_id_list;
  GeneratePuttableFourSpace<P>(sub_rest_list_key, &sub_four_space_id_list);
  
  const auto& move_four_space_id_list = GetPuttableFourSpace(rest_move);

  GeneratePuttableFourSpace<P>(move_four_space_id_list, sub_four_space_id_list, puttable_four_space_id_list);
}

template<PlayerTurn P>
const std::vector<FourSpaceID>& FourSpaceManager::GetFeasibleFourSpace(const OpenRestListKey open_rest_list_key)
{
  auto find_it = open_rest_key_feasible_four_space_id_.find(open_rest_list_key);

  if(find_it == open_rest_key_feasible_four_space_id_.end()){
    CreateOpenRestKey<P>(open_rest_list_key);

    find_it = open_rest_key_feasible_four_space_id_.find(open_rest_list_key);
    assert(find_it != open_rest_key_feasible_four_space_id_.end());
  }

  return find_it->second;
}

template<PlayerTurn P>
inline const std::vector<FourSpaceID>& FourSpaceManager::GetFeasibleFourSpace(const OpenRestList &open_rest_list)
{
  const auto open_rest_key = open_rest_list.GetOpenRestKey();
  return GetFeasibleFourSpace<P>(open_rest_key);
}

template<PlayerTurn P>
void FourSpaceManager::GenerateFeasibleFourSpace(const std::vector<FourSpaceID> &puttable_four_space_list, std::vector<FourSpaceID> * const feasible_four_space_list)
{
  assert(feasible_four_space_list != nullptr);
  assert(feasible_four_space_list->empty());

  feasible_four_space_list->reserve(puttable_four_space_list.size());

  for(const auto four_space_id : puttable_four_space_list){
    if(!IsFeasibleFourSpace<P>(four_space_id)){
      continue;
    }

    feasible_four_space_list->emplace_back(four_space_id);
  }
}

template<PlayerTurn P>
const bool FourSpaceManager::IsFiveFourSpace(const FourSpaceID four_space_id)
{
  const auto find_it = five_check_result_.find(four_space_id);

  if(find_it != five_check_result_.end()){
    return find_it->second;
  }

  static constexpr PlayerTurn Q = GetOpponentTurn(P);
  static constexpr PositionState S = GetPlayerStone(P);
  static constexpr PositionState T = GetPlayerStone(Q);

  const auto& four_space = GetFourSpace(four_space_id);
  bit_board_.SetState<S>(four_space.GetGainBit());
  bit_board_.SetState<T>(four_space.GetCostBit());

  const bool is_five = bit_board_.IsFiveStones<P>() || bit_board_.IsFiveStones<Q>();

  bit_board_.SetState<kOpenPosition>(four_space.GetGainBit() | four_space.GetCostBit());

  five_check_result_.insert(std::make_pair(four_space_id, is_five));

  return is_five;
}

template<PlayerTurn P>
const bool FourSpaceManager::IsFeasibleFourSpace(const FourSpaceID four_space_id)
{
  const auto& four_space = GetFourSpace(four_space_id);
  assert(four_space.IsPuttable());

  if(!four_space.IsBalanced()){
    return false;
  }

  return true;
}

template<PlayerTurn P>
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
      GeneratePuttableFourSpace<P>(check_puttable_list_1, check_puttable_list_2, &check_puttable_list);

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

template<PlayerTurn P>
void FourSpaceManager::IsFeasibleConsistent()
{
  for(const auto& element : open_rest_key_puttable_four_space_id_){
    const auto open_rest_key = element.first;
    const auto& puttable_four_space_id_list = element.second;

    std::vector<FourSpaceID> check_feasible_four_space_id_list;
    GenerateFeasibleFourSpace<P>(puttable_four_space_id_list, &check_feasible_four_space_id_list);

    const auto find_it = open_rest_key_feasible_four_space_id_.find(open_rest_key);

    if(find_it == open_rest_key_feasible_four_space_id_.end()){
      std::cerr << "[error]Feasible inconsistent: key not found" << std::endl;
      continue;
    }

    const auto& feasible_four_space_id_list = find_it->second;

    if(feasible_four_space_id_list.size() != check_feasible_four_space_id_list.size()){
      std::cerr << "[error]Feasible inconsistent: size is not same at key = " << open_rest_key << std::endl;
      continue;
    }

    for(const auto four_space_id : check_feasible_four_space_id_list){
      const auto check_it = find(feasible_four_space_id_list.begin(), feasible_four_space_id_list.end(), four_space_id);

      if(check_it == feasible_four_space_id_list.end()){
        std::cerr << "[error]Feasible inconsistent: four space id(" << four_space_id << ") is not contained" << std::endl;
        break;
      }
    }
  }
}

inline const FourSpace& FourSpaceManager::GetFourSpace(const FourSpaceID four_space_id) const
{
  assert(!IsInfeasibleFourSpace(four_space_id));
  assert(four_space_id < four_space_list_.size());
  
  return four_space_list_[four_space_id];
}

inline void FourSpaceManager::AddOpenRestListKey(const OpenRestListKey open_rest_list_key)
{
  open_rest_dependency_.Add(open_rest_list_key);
}

inline const size_t FourSpaceManager::GetFourSpaceCount(const MovePosition move) const
{
  const auto find_it = open_rest_key_feasible_four_space_id_.find(move);

  if(find_it == open_rest_key_feasible_four_space_id_.end()){
    return 0;
  }

  const auto& four_space_id_list = find_it->second;

  return four_space_id_list.size();
}

inline const bool FourSpaceManager::InsertList(std::vector<FourSpaceID> &list, const FourSpaceID four_space_id) const
{
  const auto find_it = find(list.begin(), list.end(), four_space_id);

  if(find_it == list.end()){
    list.emplace_back(four_space_id);
    return true;
  }

  return false;
}

}   // namespace realcore

#endif    // FOUR_SPACE_MANAGER_H
