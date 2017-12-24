//! @file
//! @brief 獲得/損失空間を管理するクラス
//! @author Koichi NABETANI
//! @date 2017/12/22

#ifndef FOUR_SPACE_MANAGER_H
#define FOUR_SPACE_MANAGER_H

#include <vector>
#include <deque>
#include <map>

#include "FourSpace.h"
#include "OpenRestList.h"
#include "OpenRestDependency.h"
#include "BitBoard.h"

namespace realcore
{

typedef size_t FourSpaceID;   //! FourSpaceを一意に特定する
static constexpr FourSpaceID kInvalidFourSpaceID = 0;   //! 無効を表すFourSpaceID

typedef std::pair<OpenRestListKey, FourSpaceID> RestKeyFourSpace;   //! (開残路キー, FourSpaceID)

class FourSpaceManagerTest;

class FourSpaceManager
{
  friend class FourSpaceManagerTest;

public:
  FourSpaceManager(const MoveList &board_move_list);

  //! @brief FourSpaceを取得する
  const FourSpace& GetFourSpace(const FourSpaceID four_space_id) const;

  //! @brief FourSpaceを追加する
  //! @param added_four_space_list 新規に追加した(開残路キー, FourSpaceID)のリスト
  template<PlayerTurn P>
  void AddFourSpace(const MovePosition gain_move, const MovePosition cost_move, const FourSpace &four_space, std::vector<RestKeyFourSpace> * const added_four_space_list);

  //! @brief 開残路キーを追加する
  //! @note AddFourSpaceでRelaxedFourオブジェクトを受け取るようにすれば不要？
  void AddOpenRestListKey(const OpenRestListKey open_rest_list_key);

private:
  //! @brief 開残路のFourSpaceを追加する
  template<PlayerTurn P>
  void AddOpenRestListFourSpace(const OpenRestListKey open_rest_list_key, const FourSpaceID four_space_id, std::vector<RestKeyFourSpace> * const added_four_space_list);

  //! @brief 2つの獲得/損失空間のIDリストから同時設置可能な獲得/損失空間のリストを生成する
  //! @param four_space_id_list_1 獲得/損失空間のIDリスト
  //! @param four_space_id_list_2 獲得/損失空間のIDリスト
  //! @param puttable_four_space 同時設置可能な獲得/損失空間のリストの格納先
  template<PlayerTurn P>
  void GeneratePuttableFourSpace(const std::vector<FourSpaceID> &four_space_id_list_1, const std::vector<FourSpaceID> &four_space_id_list_2, std::vector<FourSpaceID> * const puttable_four_space_id_list);
  
  //! @brief 同一のFourSpaceを検索する
  //! @retval kInvalidFourSpaceID: 同一のFourSpaceが存在せず
  const FourSpaceID GetFourSpaceID(const FourSpace &four_space) const;

  //! @brief FourSpaceを登録する
  //! @pre four_spaceは未登録であること
  const FourSpaceID RegisterFourSpace(const FourSpace &four_space);

  //! @brief 開残路にFourSpaceを登録する
  //! @param open_rest_list_key 開残路キー
  //! @param four_space_id FourSpaceID
  //! @retval true 登録
  //! @retval false 登録済
  const bool RegisterOpenRestKeyFourSpace(const OpenRestListKey open_rest_list_key, const FourSpaceID four_space_id);

  //! @brief 指定位置の獲得/損失空間のリストを取得する
  const std::vector<FourSpaceID>& GetFourSpaceIDList(const OpenRestListKey open_rest_list_key) const;

  std::deque<FourSpace> four_space_list_;   //! FourSpaceID -> FourSpacePtrを保持するリスト
  // todo implement transposition table

  OpenRestDependency open_rest_dependency_;   //! 開残路の依存関係を管理する
  std::map<OpenRestListKey, std::vector<FourSpaceID>> open_rest_key_four_space_id_;    //! 開残路キー -> FourSpaceIDのリスト

  BitBoard bit_board_;
};  // class FourSpaceManager


}   // namespace realcore

#include "FourSpaceManager-inl.h"

#endif    // FOUR_SPACE_MANAGER_H