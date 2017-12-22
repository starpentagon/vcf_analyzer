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
  FourSpaceManager();

  //! @brief FourSpaceを取得する
  const FourSpace& GetFourSpace(const FourSpaceID four_space_id) const;

  //! @brief FourSpaceを追加する
  //! @param added_four_space_list 新規に追加した(開残路キー, FourSpaceID)のリスト
  void AddFourSpace(const MovePosition gain_move, const MovePosition cost_move, const FourSpace &four_space, std::vector<RestKeyFourSpace> * const added_four_space_list);

  //! @brief 開残路キーを追加する
  void AddOpenRestListKey(const OpenRestListKey open_rest_list_key);

private:
  //! @brief 同一のFourSpaceを検索する
  //! @retval kInvalidFourSpaceID: 同一のFourSpaceが存在せず
  const FourSpaceID GetFourSpaceID(const MovePosition move, const FourSpace &four_space) const;

  std::deque<FourSpace> four_space_list_;   //! FourSpaceID -> FourSpacePtrを保持するリスト

  OpenRestDependency open_rest_dependency_;   //! 開残路の依存関係を管理する
  std::map<OpenRestListKey, std::vector<FourSpaceID>> open_rest_key_four_space_id_;    //! 開残路キー -> FourSpaceIDのリスト
};  // class FourSpaceManager


}   // namespace realcore

#endif    // FOUR_SPACE_MANAGER_H