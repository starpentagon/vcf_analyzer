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

  // todo delete --
  IsFourSpaceConsistent<P>();
  // -- todo delete
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
  //std::cerr << "New: key: " << gain_move << " = " << GetOpenRestKeyString(gain_move) << ", id: " << four_space_id << std::endl;

  // open_rest_list_keyに依存する開残路キーのFourSpaceリストを更新する
  std::set<OpenRestListKey> child_rest_key_set;
  open_rest_dependency_.GetAllDependentKeys(gain_move, &child_rest_key_set);
  
  if(child_rest_key_set.empty()){
    return;
  }

  std::vector<FourSpaceID> four_space_id_list{four_space_id};
  
  for(const auto child_rest_key : child_rest_key_set){
    std::vector<MovePosition> child_rest_move_list;
    GetOpenRestMoveList(child_rest_key, &child_rest_move_list);

    const auto find_it = find(child_rest_move_list.begin(), child_rest_move_list.end(), gain_move);
    assert(find_it != child_rest_move_list.end());
    child_rest_move_list.erase(find_it);

    OpenRestList child_open_rest_list(child_rest_move_list);
    const auto sub_child_key = child_open_rest_list.GetOpenRestKey();

    const auto child_it = open_rest_key_puttable_four_space_id_.find(sub_child_key);
  
    if(child_it == open_rest_key_puttable_four_space_id_.end()){
      CreateOpenRestKey<P>(sub_child_key);
    }

    const auto &child_four_space_id_list = GetPuttableFourSpace(sub_child_key);

    std::vector<FourSpaceID> puttable_four_space_id_list;
    GeneratePuttableFourSpace(four_space_id_list, child_four_space_id_list, &puttable_four_space_id_list);

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
  // 開残路キーが未登録の場合は全組合せを生成する
  assert(open_rest_key_puttable_four_space_id_.find(open_rest_list_key) == open_rest_key_puttable_four_space_id_.end());

  std::vector<FourSpaceID> puttable_four_space_id_list;
  std::vector<FourSpaceID> feasible_four_space_id_list;

  if(open_rest_list_key > kMoveNum){
    // 開残路が2つ以上の場合は組合せを生成する
    GeneratePuttableFourSpace(open_rest_list_key, &puttable_four_space_id_list);
    GenerateFeasibleFourSpace<P>(puttable_four_space_id_list, &feasible_four_space_id_list);
  }

  open_rest_key_puttable_four_space_id_.insert(
    make_pair(open_rest_list_key, puttable_four_space_id_list)
  );

  open_rest_key_feasible_four_space_id_.insert(
    make_pair(open_rest_list_key, feasible_four_space_id_list)
  );

  open_rest_dependency_.Add(open_rest_list_key);
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

template<PlayerTurn P>
void FourSpaceManager::IsFourSpaceConsistent()
{
  // todo implement
}

inline const std::vector<FourSpaceID>& FourSpaceManager::GetPuttableFourSpace(const OpenRestListKey open_rest_list_key) const
{
  const auto find_it = open_rest_key_puttable_four_space_id_.find(open_rest_list_key);
  assert(find_it != open_rest_key_puttable_four_space_id_.end());

  return find_it->second;
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
const bool FourSpaceManager::IsFeasibleFourSpace(const FourSpaceID four_space_id)
{
  const auto& four_space = GetFourSpace(four_space_id);
  assert(four_space.IsPuttable());

  if(!four_space.IsBalanced()){
    return false;
  }

  // todo balanced checkの結果もresultとして保持した方が速いかは要計測
  const auto find_it = feasibility_result_.find(four_space_id);

  if(find_it != feasibility_result_.end()){
    return find_it->second;
  }

  static constexpr PlayerTurn Q = GetOpponentTurn(P);
  static constexpr PositionState S = GetPlayerStone(P);
  static constexpr PositionState T = GetPlayerStone(Q);

  bit_board_.SetState<S>(four_space.GetGainBit());
  bit_board_.SetState<T>(four_space.GetCostBit());

  const bool is_five = bit_board_.IsFiveStones<P>() || bit_board_.IsFiveStones<Q>();

  bit_board_.SetState<kOpenPosition>(four_space.GetGainBit() | four_space.GetCostBit());

  feasibility_result_.insert(std::make_pair(four_space_id, !is_five));

  if(is_five){
    return false;
  }
  
  return true;
}

inline const FourSpace& FourSpaceManager::GetFourSpace(const FourSpaceID four_space_id) const
{
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
