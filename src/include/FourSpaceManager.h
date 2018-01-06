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
class FourSpaceSearchTest;

class FourSpaceManager
{
  friend class FourSpaceManagerTest;
  friend class FourSpaceSearchTest;

public:
  FourSpaceManager(const BitBoard &bit_board);

  //! @brief FourSpaceを取得する
  const FourSpace& GetFourSpace(const FourSpaceID four_space_id) const;

  //! @brief FourSpaceを追加する
  //! @param added_four_space_list 新規に追加した(開残路キー, 均等設置可能なFourSpaceID)のリスト
  //! @note すでに生成済みの開残路キーのFourSpaceを更新する
  template<PlayerTurn P>
  void AddFourSpace(const MovePosition gain_move, const MovePosition cost_move, const FourSpace &four_space, std::vector<RestKeyFourSpace> * const added_balanced_puttable_four_space_list);

  //! @brief 開残路の実現可能なFourSpaceを取得する
  template<PlayerTurn P>
  const std::vector<FourSpaceID>& GetFeasibleFourSpace(const OpenRestListKey open_rest_list_key);
  
  template<PlayerTurn P>
  const std::vector<FourSpaceID>& GetFeasibleFourSpace(const OpenRestList &open_rest_list);

  //! @brief 開残路キーを追加する
  //! @note AddFourSpaceでRelaxedFourオブジェクトを受け取るようにすれば不要？
  void AddOpenRestListKey(const OpenRestListKey open_rest_list_key);

  //! @brief 緩和四ノビの最長回数
  //! @note 解禁や四ノリを防ぐためにVCF手数は緩和四ノビ最長回数を上回る可能性あり
  const size_t GetMaxRelaxedFourLength() const;

  //! @brief 位置moveごとに獲得/損失空間数を取得する
  const size_t GetFourSpaceCount(const MovePosition move) const;
    
private:
  //! @brief 位置gain_moveのFourSpaceを追加する
  template<PlayerTurn P>
  void AddOpenRestListFourSpace(const MovePosition gain_move, const FourSpaceID four_space_id, std::vector<RestKeyFourSpace> * const added_four_space_list);

  //! @brief 開残路キーに対応するFourSpaceを生成する
  template<PlayerTurn P>
  void CreateOpenRestKey(const OpenRestListKey open_rest_list_key);

  //! @brief 設置可能な獲得/損失空間を取得する
  //! @pre 開残路キーに対応するFourSpaceは生成済であること
  const std::vector<FourSpaceID>& GetPuttableFourSpace(const OpenRestListKey open_rest_list_key) const;

  //! @brief 2つの獲得/損失空間のIDリストから同時設置可能な獲得/損失空間のリストを生成する
  //! @param four_space_id_set_1 獲得/損失空間のIDリスト
  //! @param four_space_id_set_2 獲得/損失空間のIDリスト
  //! @param puttable_four_space 同時設置可能な獲得/損失空間のリストの格納先
  //! @note 均等(gain, costの石数)チェック, 五連以上チェックは行わない
  template<PlayerTurn P>
  void GeneratePuttableFourSpace(const std::vector<FourSpaceID> &four_space_id_list_1, const std::vector<FourSpaceID> &four_space_id_list_2, std::vector<FourSpaceID> * const puttable_four_space_id_list);
  
  //! @brief 開残路キーに対応する同時設置可能な獲得/損失空間のリストを生成する
  template<PlayerTurn P>
  void GeneratePuttableFourSpace(const OpenRestListKey open_rest_list_key, std::vector<FourSpaceID> * const puttable_four_space_id_list);

  //! @brief 設置可能な獲得/損失空間から実現可能な獲得/損失空間を生成する
  //! @note 均等(gain, costの石数)チェックを行う
  template<PlayerTurn P>
  void GenerateFeasibleFourSpace(const std::vector<FourSpaceID> &puttable_four_space_list, std::vector<FourSpaceID> * const feasible_four_space_list);

  //! @brief FourSpaceが実現可能(均等(gain, costの同数))か判定する
  //! @pre FourSpaceは設置可能であること
  template<PlayerTurn P>
  const bool IsFeasibleFourSpace(const FourSpaceID four_space_id);

  //! @brief FourSpaceに五連以上が生じているかチェックする
  template<PlayerTurn P>
  const bool IsFiveFourSpace(const FourSpaceID four_space_id);

  //! @brief 同一のFourSpaceを検索する
  //! @retval kInvalidFourSpaceID: 同一のFourSpaceが存在せず
  const FourSpaceID GetFourSpaceID(const FourSpace &four_space) const;

  //! @brief FourSpaceを登録する
  //! @pre four_spaceは未登録であること
  //! @note 五連以上が生じているかのチェックを行う
  template<PlayerTurn P>
  const FourSpaceID RegisterFourSpace(const FourSpace &four_space);

  //! @brief FourSpaceが実現不可能（五連以上が生じている）かを返す
  const bool IsInfeasibleFourSpace(const FourSpaceID four_space_id) const;

  //! @brief 開残路にFourSpaceを登録する
  //! @param open_rest_list_key 開残路キー
  //! @param four_space_id FourSpaceID
  //! @retval true 登録
  //! @retval false 登録済
  //! @note FourSpaceは設置可能であること
  template<PlayerTurn P>
  const bool RegisterOpenRestKeyFourSpace(const OpenRestListKey open_rest_list_key, const FourSpaceID four_space_id);

  //! @brief 開残路にFourSpaceを登録する
  //! @param open_rest_list_key 開残路キー
  //! @param four_space_id_list 設置可能なFourSpaceIDのリスト
  template<PlayerTurn P>
  void RegisterOpenRestKeyFourSpace(const OpenRestListKey open_rest_list_key, const std::vector<FourSpaceID> &four_space_id_list);

  //! @brief 設置可能なFourSpaceの依存関係（子が親から生成できているか）をチェックする
  //! @note デバッグ用機能
  template<PlayerTurn P>
  void IsPuttableConsistent();

  //! @brief 設置可能なFourSpaceから実現可能なFourSpaceが生成できるかチェックする
  //! @note デバッグ用機能
  template<PlayerTurn P>
  void IsFeasibleConsistent();

  //! @brief List中に要素がない場合に挿入する
  //! @retval true: 要素を挿入, false: すでに要素があり挿入せず
  const bool InsertList(std::vector<FourSpaceID> &list, const FourSpaceID four_space_id) const;

  //! @brief 親の開残路キーを取得する
  //! @retval (親から指し手, 親の開残路キー, 親の開残路キーが登録済かのフラグ)
  const std::tuple<MovePosition, OpenRestListKey, bool> GetParentOpenRestListKey(const OpenRestListKey open_rest_list_key) const;

  //! @brief 設置可能なFourSpaceのサマリ情報を出力する
  void OutputPuttableSummary() const;

  //! @brief 実現可能なFourSpaceのサマリ情報を出力する
  void OutputFeasibleSummary() const;

  //! FourSpaceID -> FourSpacePtrを保持するリスト
  //! @note unique_ptr<FourSpace>をvector型で保持した場合より2%程度高速
  std::deque<FourSpace> four_space_list_;
  std::multimap<size_t, FourSpaceID> four_space_hash_table_;    //! FourSpaceIDのHashTable

  OpenRestDependency open_rest_dependency_;   //! 開残路の依存関係を管理する

  //! @brief 開残路キー -> 設置可能なFourSpaceIDのリスト
  std::map<OpenRestListKey, std::vector<FourSpaceID>> open_rest_key_puttable_four_space_id_;

  //! @brief 開残路キー -> 実現可能(設置可能、均等、五連以上なし)なFourSpaceIDのリスト
  std::map<OpenRestListKey, std::vector<FourSpaceID>> open_rest_key_feasible_four_space_id_;

  //! @brief FourSpaceID -> 五連チェック結果
  std::map<FourSpaceID, bool> five_check_result_;

  BitBoard bit_board_;

  //! @brief 設置可能性のチェック結果
  size_t puttable_check_count_;
  size_t puttable_count_;
};  // class FourSpaceManager


}   // namespace realcore

#include "FourSpaceManager-inl.h"

#endif    // FOUR_SPACE_MANAGER_H