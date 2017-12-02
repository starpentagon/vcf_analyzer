//! @file
//! @brief Four Space Searchを行うクラス
//! @author Koichi NABETANI
//! @date 2017/08/15
#ifndef FOUR_SPACE_SEARCH_H
#define FOUR_SPACE_SEARCH_H

#include <tuple>
#include <array>
#include <vector>
#include <set>
#include <map>
#include <memory>

#include "RelaxedFour.h"
#include "BitBoard.h"
#include "HashTable.h"
#include "FourSpace.h"

namespace realcore{

//! 指し手位置ごとに緩和四ノビIDのリストを保持するテーブル
typedef std::array<std::vector<RelaxedFourID>, kMoveNum> MoveRelaxedFourIDList;

//! 指し手位置ごとに緩和四ノビ生成でチェック済みの直線近傍パターンを保持するテーブル
typedef std::array<std::vector<LocalBitBoard>, kMoveNum> MoveLocalBitBoardList;

//! 緩和四ノビIDを保持するsetのsmart pointer
typedef std::unique_ptr<std::set<RelaxedFourID> > RelaxedFourIDSetPtr;

//! 獲得/損失空間リストのsmart pointer
typedef std::unique_ptr<std::vector<FourSpace>> FourSpaceVectorPtr;

//! 緩和四ノビIDリストのsmart pointer
typedef std::unique_ptr<std::vector<RelaxedFourID>> RelaxedFourIDVectorPtr;

//! 獲得路から新たな緩和四ノビする際の情報を保持するtuple
//! @note 1要素目: gain
//! @note 2要素目: cost
//! @note 3要素目: rest_1(残路)
//! @note 4要素目: rest_2(残路)
//! @note 5要素目: rest_3(残路)
typedef std::tuple<MovePosition, MovePosition, MovePosition, MovePosition, MovePosition> NextRelaxedFourInfo;

//! 開残路へ到達可能な到達路のRelaxedFourID
typedef std::pair<RelaxedFourID, RelaxedFourID> RestGainFourID;

//! 開残路リストのkey
typedef uint64_t RestListKey;

class FourSpaceSearchTest;

class FourSpaceSearch
: public BitBoard
{
  friend class FourSpaceSearchTest;

public:
  FourSpaceSearch(const BitBoard &bit_board);

  //! @brief 四追い空間を展開する
  //! @note 初期局面での相手の四ノビは考慮しない
  template<PlayerTurn P>
  void ExpandFourSpace();
  void ExpandFourSpace(const bool is_black_turn);

  //! @brief 四追い空間を展開する(四ノビ指定版)
  template<PlayerTurn P>
  void ExpandFourSpace(const std::vector<MovePair> &four_list);
  void ExpandFourSpace(const bool is_black_turn, const std::vector<MovePair> &four_list);

  //! @brief 緩和四ノビの数
  const size_t GetRelaxedFourCount() const;

  //! @brief 緩和四ノビの最長回数
  //! @note 解禁や四ノリを防ぐためにVCF手数は緩和四ノビ最長回数を上回る可能性あり
  const size_t GetMaxRelaxedFourLength() const;

private:
  const RelaxedFour& GetRelaxedFour(const RelaxedFourID relaxed_four_id) const;
  RelaxedFour& GetRelaxedFour(const RelaxedFourID relaxed_four_id);

  //! @brief 2つの獲得/損失空間のリストから同時設置可能な獲得/損失空間のリストを生成する
  //! @param four_space_list_1 獲得/損失空間のリスト
  //! @param four_space_list_2 獲得/損失空間のリスト
  //! @param puttable_four_space 同時設置可能な獲得/損失空間のリストの格納先
  void GeneratePuttableFourSpace(const std::vector<FourSpace> &four_space_list_1, const std::vector<FourSpace> &four_space_list_2, std::vector<FourSpace> * const puttable_four_space_list) const;

  //! @brief 位置moveへの獲得/損失空間追加による緩和四ノビを生成する
  //! @param gain_position 獲得/損失空間が追加された位置
  //! @param four_space 追加した獲得/損失空間
  //! @param additional_relaxed_four 追加した緩和四ノビ
  template<PlayerTurn P>
  void GenerateRelaxedFour(const MovePosition gain_position, const FourSpace &four_space, std::vector<RelaxedFourID> * const additional_relaxed_four);

  //! @brief Relaxed Fourのデータを追加する
  //! @retval RelaxedFourID, true: 新たに追加, false: すでに生成済み
  const std::pair<RelaxedFourID, bool> AddRelaxedFour(const RelaxedFour &relaxed_four);
  const std::pair<RelaxedFourID, bool> AddRelaxedFour(const NextRelaxedFourInfo &next_relaxed_four);

  //! @brief 到達路1つ + 自石2つでRelaxed Fourを作る手を列挙する
  template<PlayerTurn P>
  void GetRelaxedFourFromOneGainPosition(const BoardOpenState &board_open_state, std::vector<NextRelaxedFourInfo> * const next_four_info_list) const;

  //! @brief 到達路2つ + 自石1つでRelaxed Fourを作る手を列挙する
  //! @param additional_gain 追加で着手した到達路(到達路3つからRelaxed Four生成用)
  template<PlayerTurn P>
  void GetRelaxedFourFromTwoGainPosition(const BoardOpenState &board_open_state, const MovePosition additional_gain, std::vector<NextRelaxedFourInfo> * const next_four_info_list) const;

  //! @brief 到達路3つでRelaxed Fourを作る手を列挙する
  template<PlayerTurn P>
  void GetRelaxedFourFromThreeGainPosition(const MovePosition gain_position, std::vector<NextRelaxedFourInfo> * const next_four_info_list);

  //! @brief 指定位置の特定方向の近傍(開残路位置)に存在する獲得路を求める
  //! @param gain_position 指定位置
  //! @param direction 方向
  //! @param restable_four_id_list 開残路位置にあるRelaxed FourID
  //! @retval 開残路位置の数
  size_t GetRestableRelaxedFourIDList(const MovePosition gain_position, const BoardDirection direction, std::vector<RelaxedFourID> * const restable_four_id_list) const;

  //! @brief 指定位置の獲得/損失空間のリストを取得する
  const std::vector<FourSpace>& GetFourSpaceList(const MovePosition move) const;

  //! @brief 指定位置の獲得/損失空間を追加する
  template<PlayerTurn P>
  void AddFourSpace(const MovePosition move, const FourSpace &four_space);

  //! @brief 獲得/損失空間がすでに登録されているかチェックする
  const bool IsRegisteredFourSpace(const MovePosition move, const FourSpace &four_space) const;

 //! @brief 直線近傍パターンが緩和四ノビ生成でチェック済みかどうかを返す
 const bool IsRegisteredLocalBitBoard(const MovePosition move, const LocalBitBoard &local_bitboard) const;

  //! @brief 開残路リストのkeyを取得する
  const RestListKey GetOpenRestKey(std::vector<MovePosition> &rest_list) const;

  //! @brief 位置moveのmove位置を除く開残路位置を取得する
  void GetRestPosition(const MovePosition move, RestListKey rest_list_key, std::vector<MovePosition> * const rest_list) const;

  //! @brief 開残路位置を取得する
  void GetRestPosition(RestListKey rest_list_key, std::vector<MovePosition> * const rest_list) const;

  //! @brief 開残路リストのkeyがmoveを含むかチェックする
  const bool IsMoveInRestPosition(const RestListKey rest_list_key, const MovePosition move) const;

  //! @brief 獲得/損失空間の追加による開残路リストごとの同時設置可能な獲得/損失空間の増分を反映する
  //! @param move 獲得/損失空間が追加された位置
  //! @param four_space 追加した獲得/損失空間
  //! @param additional_four_space_list 同時設置可能な獲得/損失空間の格納先
  template<PlayerTurn P>
  void UpdateAdditionalPuttableFourSpace(const MovePosition move, const FourSpace &four_space, const std::map<RestListKey, std::vector<FourSpace>> &additional_four_space);

  //! @brief 獲得/損失空間の追加による開残路リストごとの同時設置可能な獲得/損失空間の増分を反映する
  //! @param rest_key 獲得/損失空間が追加された残路リストのキー
  //! @param four_space 追加した獲得/損失空間
  //! @param additional_four_space_list 同時設置可能な獲得/損失空間の格納先
  template<PlayerTurn P>
  void UpdateRestListPuttableFourSpace(const RestListKey rest_key, const FourSpace &four_space, std::map<RestListKey, std::vector<FourSpace>> * const additional_four_space);

  //! @brief 獲得/損失空間を追加する
  //! @retval true 追加, false 登録済みや実現不可能なため追加せず
  template<PlayerTurn P>
  const bool AddRestListFourSpace(const RestListKey rest_key, const FourSpace &four_space, const bool is_register_check);

  template<PlayerTurn P>
  void AddRestListFourSpace(const RestListKey rest_key, const std::vector<FourSpace> &four_space_list, const bool is_register_check, std::vector<FourSpace> * const added_four_space_list);

  //! @brief 開残路の獲得/損失空間リストに対して緩和四ノビを展開する
  template<PlayerTurn P>
  void ExpandRelaxedFour(const RelaxedFourID relaxed_four_id, const std::vector<FourSpace> &rest_four_space_list);

  //! @brief 獲得/損失空間の追加による開残路リストごとの同時設置可能な獲得/損失空間を列挙する
  //! @param rest_list 開残路リスト
  //! @param additional_four_space_list 同時設置可能な獲得/損失空間の格納先
  template<PlayerTurn P>
  void EnumeratePuttableFourSpace(const std::vector<MovePosition> &rest_list, std::vector<FourSpace> * const puttable_four_space_list);
  
  //! @brief 開残路リストに登録ずみの獲得/損失空間かどうかをチェックする
  const bool IsRegisteredFourSpace(const RestListKey rest_key, const FourSpace &four_space) const;

  //! @brief sub_rest_keyとsuper_rest_keyの差分位置を取得する
  const MovePosition GetAdditionalMove(const RestListKey sub_rest_key, const RestListKey super_rest_key) const;
    
  //! @brief 残路リスト/残路リストキーを取得する
  const RestListKey GetRestList(const NextRelaxedFourInfo &next_four_info, std::vector<MovePosition> * const rest_list) const;

  //! @brief 残路リストの長さを返す
  const size_t GetRestListSize(const RestListKey rest_key) const;

  //! @brief MoveBitSetの内容をBitBoardに反映する
  template<PositionState S>
  void SetMoveBit(const MoveBitSet &move_bit);

  //! @brief 位置moveごとに緩和四ノビ数を出力する
  void ShowBoardRelaxedFourCount() const;

  //! @brief 位置moveごとに獲得/損失空間数を出力する
  void ShowBoardGainCostSpaceCount() const;

  //! @brief 位置moveが獲得路になる緩和四ノビIDのリスト
  //! @note 緩和四ノビは到達可能なものに限定
  void GetGainMoveRelaxedFourIDList(MoveRelaxedFourIDList * const move_relaxed_four_id_list) const;

  //! @brief 位置moveが損失路になる緩和四ノビIDのリスト
  //! @note 緩和四ノビは到達可能なものに限定
  void GetCostMoveRelaxedFourIDList(MoveRelaxedFourIDList * const move_relaxed_four_id_list) const;

  //! @brief 獲得路が位置moveとなるR-四ノビ可能な緩和四ノビIDを追加する
  void AddFeasibleRelaxedFourID(const RelaxedFourID relaxed_four_id);

  //! @brief 開残路キーの依存木を更新する
  void UpdateRestListKeyTree(const std::vector<MovePosition> &rest_list);
  void UpdateRestListKeyTree(const RestListKey rest_list_key);

  std::vector<std::unique_ptr<RelaxedFour>> relaxed_four_list_;   //! relaxed_four_list_[RelaxedFourID] -> RelaxedFourIDに対応するRelaxedFourのデータ
  std::map<uint64_t, RelaxedFourID> transposition_table_;   //! RelaxedFourの置換表(到達路、損失路、残路が等しいRelaxedFourを同一視)
  
  std::map<MovePosition, RelaxedFourIDSetPtr> move_feasible_relaxed_four_id_list_;    //! 位置moveが獲得路になる緩和四ノビIDのリスト(R-四ノビ可能なもののみ格納する)
  
  MoveLocalBitBoardList move_local_bitboard_list_;    //! 位置moveごとの緩和四ノビ生成でチェック済みの直線近傍パターン
  
  std::map<RestListKey, FourSpaceVectorPtr> rest_list_puttable_four_space_;    //! 開残路リストkeyごとに同時設置可能な獲得/損失空間のリスト
  std::map<RestListKey, std::set<RestListKey>> rest_key_tree_;      //! 開残路キーの依存木RestListKey k -> rest_key_tree_[k] kに依存するRestListKey
  std::map<RestListKey, RelaxedFourIDVectorPtr> rest_list_relaxed_four_list_;  //! 開残路リスト -> 緩和四ノビIDのリスト

  PlayerTurn attack_player_;    //! 詰め方(黒 or 白)
};

}   // namespace realcore

#include "FourSpaceSearch-inl.h"

#endif  // FOUR_SPACE_SEARCH
