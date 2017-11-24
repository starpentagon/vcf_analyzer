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
#include <stack>

#include "RelaxedFour.h"
#include "BitBoard.h"
#include "HashTable.h"
#include "FourSpace.h"

namespace realcore{

//! 指し手位置ごとに緩和四ノビIDのリストを保持するテーブル
typedef std::array<std::vector<RelaxedFourID>, kMoveNum> MoveRelaxedFourIDList;

//! 指し手位置ごとに緩和四ノビ生成でチェック済みの直線近傍パターンを保持するテーブル
typedef std::array<std::vector<LocalBitBoard>, kMoveNum> MoveLocalBitBoardList;

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

  //! @brief 獲得/損失空間を列挙する(緩和四ノビ生成版)
  //! @param gain_position 獲得路
  //! @param cost_position 損失路
  //! @param rest_list 残路リスト
  //! @param four_space_list 同時設置可能な獲得/損失空間の格納先
  void EnumerateFourSpace(const MovePosition gain_position, const MovePosition cost_position, const std::vector<MovePosition> &rest_list, std::vector<FourSpace> * const four_space_list) const;

  //! @brief 2つの獲得/損失空間のリストから同時設置可能な獲得/損失空間のリストを生成する
  //! @param four_space_list_1 獲得/損失空間のリスト
  //! @param four_space_list_2 獲得/損失空間のリスト
  //! @param puttable_four_space 同時設置可能な獲得/損失空間のリストの格納先
  void GeneratePuttableFourSpace(const std::vector<FourSpace> &four_space_list_1, const std::vector<FourSpace> &four_space_list_2, std::vector<FourSpace> * const puttable_four_space_list) const;

  //! @brief 獲得路/損失路と獲得/損失空間リストから競合していない獲得/損失空間のリストを生成する
  //! @param gain_position 獲得路
  //! @param cost_position 損失路
  //! @param four_space_list 獲得/損失空間のリスト
  //! @param non_conflict_four_space_list 競合していない獲得/損失空間のリストの格納先
  void GenerateNonConflictFourSpace(const MovePosition gain_position, const MovePosition cost_position, const std::vector<FourSpace> &four_space_list, std::vector<FourSpace> * const non_conflict_four_space_list) const;

  //! @brief 位置moveへの獲得/損失空間追加による緩和四ノビを生成する
  //! @param gain_position 獲得/損失空間が追加された位置
  //! @param four_space 追加した獲得/損失空間
  //! @param additional_four_space gain_positionを含む残路リストの同時設置可能な獲得/損失空間リスト
  template<PlayerTurn P>
  void GenerateRelaxedFour(const MovePosition gain_position, const FourSpace &four_space, const std::map<RestListKey, std::vector<FourSpace>> &additional_four_space);

  //! @brief 獲得路が指定位置になるRelaxedFourIDのリストを取得する
  const std::vector<RelaxedFourID>& GetGainRelaxedFourIDList(const MovePosition gain_position) const;

  //! @brief Relaxed Fourのデータを追加する
  const RelaxedFourID AddRelaxedFour(const RelaxedFour &relaxed_four);
  const RelaxedFourID AddRelaxedFour(const MovePosition gain_position, const MovePosition cost_position, const std::vector<MovePosition> &rest_list);

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

  //! @brief 指定位置を残路に持つ緩和四ノビIDリストを取得する
  void GetRestRelaxedFourIDList(const MovePosition move, std::vector<RelaxedFourID> * const rest_relaxed_four_id_list) const;

  //! @brief 指定位置の獲得/損失空間を追加する
  template<PlayerTurn P>
  void AddFourSpace(const MovePosition move, const FourSpace &four_space);

  //! @brief 獲得/損失空間がすでに登録されているかチェックする
  const bool IsRegisteredFourSpace(const MovePosition move, const FourSpace &four_space) const;

  //! @brief 直線近傍パターンが緩和四ノビ生成でチェック済みかどうかを返す
  const bool IsRegisteredLocalBitBoard(const MovePosition move, const LocalBitBoard &local_bitboard) const;

  //! @brief 末端ノード(どの緩和四ノビの残路になっていない緩和四ノビ)を列挙する
  void EnumerateLeaf(std::vector<RelaxedFourID> * const leaf_id_list) const;

  template<PositionState>
  void SetState(const MovePosition move);

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
  void UpdateRestListPuttableFourSpace(const RestListKey rest_key, const FourSpace &four_space, std::map<RestListKey, std::vector<FourSpace>> * const additional_four_space);

  //! @brief 獲得/損失空間を追加する
  void AddRestListFourSpace(const RestListKey rest_key, const FourSpace &four_space);
  void AddRestListFourSpace(const RestListKey rest_key, const std::vector<FourSpace> &four_space_list);

  //! @brief 獲得/損失空間の追加による開残路リストごとの同時設置可能な獲得/損失空間を列挙する
  //! @param move 獲得/損失空間が追加された位置
  //! @param four_space 追加した獲得/損失空間
  //! @param additional_four_space_list 同時設置可能な獲得/損失空間の格納先
  //! @brief 生成した獲得/損失空間がすでに生成済みかどうかのチェックを行わず生成した獲得/損失空間をすべて格納する
  void EnumeratePuttableFourSpace(const MovePosition move, const FourSpace &four_space, const std::vector<MovePosition> &rest_list, std::vector<FourSpace> * const puttable_four_space_list);

  //! @brief 開残路リストに登録ずみの獲得/損失空間かどうかをチェックする
  const bool IsRegisteredFourSpace(const RestListKey rest_key, const FourSpace &four_space) const;

  //! @brief sub_rest_keyがsuper_rest_keyが厳密に含まれている(sub_rest_key \subset super_rest_key, sub_rest_key \ne super_rest_key)かチェックする
  //! @retval sub_rest_keyがsuper_rest_keyに含まれており要素数の差が1の場合にtrue
  //! @param additional_move sub_rest_keyとsuper_rest_keyの差分
  const bool IsProperSubset(const RestListKey sub_rest_key, const RestListKey super_rest_key, MovePosition * const additional_move) const;

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

  std::vector<RelaxedFour> relaxed_four_list_;   //! relaxed_four_list_[RelaxedFourID] -> RelaxedFourIDに対応するRelaxedFourのデータ
  std::map<uint64_t, RelaxedFourID> transposition_table_;   //! RelaxedFourの置換表(到達路、損失路、残路が等しいRelaxedFourを同一視)
  
  MoveRelaxedFourIDList move_gain_list_;    //! 位置moveが獲得路になる緩和四ノビIDのリスト
  MoveRelaxedFourIDList move_cost_list_;    //! 位置moveが損失路になる緩和四ノビIDのリスト
  MoveRelaxedFourIDList move_open_rest_list_;    //! 位置moveが開残路になる緩和四ノビIDのリスト
  
  MoveLocalBitBoardList move_local_bitboard_list_;    //! 位置moveごとの緩和四ノビ生成でチェック済みの直線近傍パターン

  std::map<RestListKey, std::vector<FourSpace>> rest_list_puttable_four_space_;    //! 開残路リストkeyごとに同時設置可能な獲得/損失空間のリスト
  std::map<RestListKey, std::set<RestListKey>> rest_key_tree_;      //! 開残路キーの依存木
  std::map<MovePosition, std::set<RestListKey>> move_rest_key_list_;   //! 位置moveを含む開残路リストkeyのリスト
  std::map<RestListKey, std::vector<RelaxedFourID>> rest_list_relaxed_four_list_;  //! 開残路リスト -> 緩和四ノビIDのリスト


  PlayerTurn attack_player_;    //! 詰め方(黒 or 白)
};

}   // namespace realcore

#include "FourSpaceSearch-inl.h"

#endif  // FOUR_SPACE_SEARCH
