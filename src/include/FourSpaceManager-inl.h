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

  if(gain_move == kMoveDC && four_space_id == 645){
    int a = 1;
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
  std::cerr << "New: key: " << gain_move << " = " << GetOpenRestKeyString(gain_move) << ", id: " << four_space_id << std::endl;

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
    std::vector<FourSpaceID> child_four_space_id_list;

    EnumeratePuttableFourSpace<P>(child_open_rest_list, &child_four_space_id_list);

    std::vector<FourSpaceID> puttable_four_space_id_list;
    GeneratePuttableFourSpace<P>(four_space_id_list, child_four_space_id_list, &puttable_four_space_id_list);

    for(const auto puttable_four_space_id : puttable_four_space_id_list){
      RegisterOpenRestKeyFourSpace<P>(child_rest_key, puttable_four_space_id);
      added_four_space_list->emplace_back(std::make_pair(child_rest_key, puttable_four_space_id));
      std::cerr << "New: key: " << child_rest_key << " = " << GetOpenRestKeyString(child_rest_key) << ", id: " << puttable_four_space_id << std::endl;
    }
  }
}

template<PlayerTurn P>
void FourSpaceManager::GeneratePuttableFourSpace(const std::vector<FourSpaceID> &four_space_id_list_1, const std::vector<FourSpaceID> &four_space_id_list_2, std::vector<FourSpaceID> * const puttable_four_space_id_list)
{
  assert(puttable_four_space_id_list != nullptr);
  assert(puttable_four_space_id_list->empty());

  static constexpr PlayerTurn Q = GetOpponentTurn(P);
  static constexpr PositionState S = GetPlayerStone(P);
  static constexpr PositionState T = GetPlayerStone(Q);

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

        const bool is_five = bit_board_.IsFiveStones<P>() || bit_board_.IsFiveStones<Q>();

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
  assert(rest_size >= 2 && rest_size <= 3);
  std::vector<FourSpaceID> generated_four_space_id_list;

  if(rest_size == 2){
    const auto& four_space_id_list_1 = GetFourSpaceIDList(rest_move_list[0]);
    const auto& four_space_id_list_2 = GetFourSpaceIDList(rest_move_list[1]);

    GeneratePuttableFourSpace<P>(four_space_id_list_1, four_space_id_list_2, &generated_four_space_id_list);
  }else if(rest_size == 3){
    std::set<FourSpaceID> generate_four_id_set;
    
    for(const auto rest_move : rest_move_list){
      std::vector<MovePosition> sub_rest_move_list(rest_move_list);
      const auto rest_it = find(sub_rest_move_list.begin(), sub_rest_move_list.end(), rest_move);
      sub_rest_move_list.erase(rest_it);

      std::vector<FourSpaceID> sub_four_space_list;
      OpenRestList sub_open_rest(sub_rest_move_list);

      EnumeratePuttableFourSpace<P>(sub_open_rest, &sub_four_space_list);
    
      const auto& move_four_space_list = GetFourSpaceIDList(rest_move);

      std::vector<FourSpaceID> four_space_id_list;
      GeneratePuttableFourSpace<P>(move_four_space_list, sub_four_space_list, &four_space_id_list);
      
      for(const auto four_space_id : four_space_id_list){
        generate_four_id_set.insert(four_space_id);
      }
    }

    generated_four_space_id_list.reserve(generate_four_id_set.size());

    for(const auto four_space_id : generate_four_id_set){
      generated_four_space_id_list.emplace_back(four_space_id);
    }
  }

  open_rest_key_four_space_id_.insert(
    make_pair(rest_list_key, generated_four_space_id_list)
  );

  open_rest_dependency_.Add(rest_list_key);

  *puttable_four_space_id_list = generated_four_space_id_list;
}

template<PlayerTurn P>
const bool FourSpaceManager::RegisterOpenRestKeyFourSpace(const OpenRestListKey open_rest_list_key, const FourSpaceID four_space_id)
{
  const auto find_it = open_rest_key_four_space_id_.find(open_rest_list_key);

  if(find_it == open_rest_key_four_space_id_.end()){
    // 開残路キーが未登録の場合は全組み合わせを生成する
    std::vector<MovePosition> rest_move_list;
    GetOpenRestMoveList(open_rest_list_key, &rest_move_list);
    const auto rest_size = rest_move_list.size();

    open_rest_dependency_.Add(open_rest_list_key);

    if(rest_size >= 2){
      std::vector<FourSpaceID> generated_four_space_id_list;
      OpenRestList open_rest_list(rest_move_list);
      EnumeratePuttableFourSpace<P>(open_rest_list, &generated_four_space_id_list);
    }else{
      const auto insert_result = open_rest_key_four_space_id_.insert(
        make_pair(open_rest_list_key, std::vector<FourSpaceID>())
      );
      assert(insert_result.second);

      auto& four_space_id_list = insert_result.first->second;
      four_space_id_list.emplace_back(four_space_id);
    }

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

template<PlayerTurn P>
void FourSpaceManager::IsFourSpaceConsistent()
{
  for(const auto& element : open_rest_key_four_space_id_){
    const auto open_rest_key = element.first;
    std::vector<MovePosition> open_rest_move;

    GetOpenRestMoveList(open_rest_key, &open_rest_move);
    const auto rest_size = open_rest_move.size();

    if(rest_size <= 1){
      continue;
    }

    const auto& four_space_id_list = element.second;

    std::set<FourSpaceID> element_four_space_id_set;

    for(const auto four_space_id : four_space_id_list){
      element_four_space_id_set.insert(four_space_id);
    }

    if(rest_size == 2){
      const auto &four_space_id_1 = GetFourSpaceIDList(open_rest_move[0]);
      const auto &four_space_id_2 = GetFourSpaceIDList(open_rest_move[1]);

      std::vector<FourSpaceID> four_space_id_list;
      GeneratePuttableFourSpace<P>(four_space_id_1, four_space_id_2, &four_space_id_list);

      assert(four_space_id_list.size() == element_four_space_id_set.size());

      for(const auto four_space_id : four_space_id_list){
        assert(element_four_space_id_set.find(four_space_id) != element_four_space_id_set.end());
      }
    }else if(rest_size == 3){
      std::set<FourSpaceID> generate_four_id_set;
      
      for(const auto rest_move : open_rest_move){
        std::vector<MovePosition> sub_rest_move_list(open_rest_move);
        const auto rest_it = find(sub_rest_move_list.begin(), sub_rest_move_list.end(), rest_move);
        sub_rest_move_list.erase(rest_it);

        std::vector<FourSpaceID> sub_four_space_list;
        OpenRestList sub_open_rest(sub_rest_move_list);

        EnumeratePuttableFourSpace<P>(sub_open_rest, &sub_four_space_list);
      
        const auto& move_four_space_list = GetFourSpaceIDList(rest_move);

        std::vector<FourSpaceID> four_space_id_list;
        GeneratePuttableFourSpace<P>(move_four_space_list, sub_four_space_list, &four_space_id_list);
        
        for(const auto four_space_id : four_space_id_list){
          generate_four_id_set.insert(four_space_id);
        }
      }

      assert(generate_four_id_set.size() == element_four_space_id_set.size());

      for(const auto four_space_id : generate_four_id_set){
        assert(element_four_space_id_set.find(four_space_id) != element_four_space_id_set.end());
      }
    }
  }
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

inline const size_t FourSpaceManager::GetFourSpaceCount(const MovePosition move) const
{
  const auto find_it = open_rest_key_four_space_id_.find(move);

  if(find_it == open_rest_key_four_space_id_.end()){
    return 0;
  }

  const auto& four_space_id_list = find_it->second;

  return four_space_id_list.size();
}

}   // namespace realcore

#endif    // FOUR_SPACE_MANAGER_H
