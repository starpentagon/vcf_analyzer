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

//! 指し手位置ごとに獲得/損失空間のリストを保持するテーブル
typedef std::array<std::vector<FourSpace>, kMoveNum> MoveFourSpaceList;

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

  //! @brief 実現手順を列挙する
  //! @retval true 実現可能な手順が存在
  //! @retval false 実現不可能
  const bool EnumerateFourSpace(const MovePosition gain_position, const MovePosition cost_position, const std::vector<MovePosition> &rest_list, std::vector<FourSpace> * const four_space_list);

  //! @brief 獲得/損失空間を列挙する(緩和四ノビ生成版)
  //! @param gain_position 獲得路
  //! @param cost_position 損失路
  //! @param rest_list 残路リスト
  //! @param four_space_list 同時設置可能な獲得/損失空間の格納先
  void EnumerateFourSpace(const MovePosition gain_position, const MovePosition cost_position, const std::vector<MovePosition> &rest_list, std::vector<FourSpace> * const four_space_list) const;

  //! @brief 獲得/損失空間を列挙する(獲得/損失空間の追加版)
  //! @param move 獲得/損失空間が追加された位置
  //! @param four_space 追加した獲得/損失空間
  //! @param relaxed_four_id 位置moveを開残路に持つ緩和四ノビID
  //! @param four_space_list 同時設置可能な獲得/損失空間の格納先
  void EnumerateFourSpace(const MovePosition move, const FourSpace &four_space, const RelaxedFourID relaxed_four_id, std::vector<FourSpace> * const four_space_list) const;

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
  template<PlayerTurn P>
  void GenerateRelaxedFour(const MovePosition gain_position, const FourSpace &four_space);

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

  //! @brief Relaxed FourIDの到達路を求める
  //! @retval true 到達路を実現可能, false 競合しており実現不可能
  bool GetReachSequence(const RelaxedFourID relaxed_four_id, MoveList * const move_list) const;
  bool GetReachSequence(const std::vector<RelaxedFourID> &four_id_list, MoveList * const move_list) const;

  //! @brief Relaxed FourIDの到達路(Relaxed FourIDリスト)を求める
  void GetReachIDSequence(const RelaxedFourID relaxed_four_id, std::vector<RelaxedFourID> * const id_list) const;
  void GetReachIDSequence(const RelaxedFourID relaxed_four_id, std::set<RelaxedFourID> * const appeared_id_set, std::vector<RelaxedFourID> * const id_list) const;

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

  //! @brief 獲得路の実現に必要な依存路の着手(獲得路/損失路)を求める
  //! @note 依存路の探索は深さ１に限定
  void GetDependentReachMove(const RelaxedFourID relaxed_four_id, std::vector<std::vector<MovePair>> * const move_pair_vector) const;

  //! @brief 獲得路に対応する損失路のリストを返す
  //! @param gain_move 獲得路
  //! @param const_move_list 損失路のリスト
  void GetCostMoveList(const MovePosition gain_move, MoveList * const cost_move_list) const;

  //! @brief 展開済みの変化かチェックする
  const bool IsExpanded(const RelaxedFourID relaxed_four_id) const;

  std::vector<RelaxedFour> relaxed_four_list_;   //! relaxed_four_list_[RelaxedFourID] -> RelaxedFourIDに対応するRelaxedFourのデータ
  std::map<uint64_t, RelaxedFourID> transposition_table_;   //! RelaxedFourの置換表(到達路、損失路、残路が等しいRelaxedFourを同一視)
  
  MoveRelaxedFourIDList move_gain_list_;    //! 位置moveが獲得路になる緩和四ノビIDのリスト
  MoveRelaxedFourIDList move_cost_list_;    //! 位置moveが損失路になる緩和四ノビIDのリスト
  MoveRelaxedFourIDList move_open_rest_list_;    //! 位置moveが開残路になる緩和四ノビIDのリスト
  
  MoveFourSpaceList move_four_space_list_;    //! 位置moveごとの局所獲得/損失空間のリスト
  MoveLocalBitBoardList move_local_bitboard_list_;    //! 位置moveごとの緩和四ノビ生成でチェック済みの直線近傍パターン

  PlayerTurn attack_player_;    //! 詰め方(黒 or 白)
};

}   // namespace realcore

#include "FourSpaceSearch-inl.h"

#endif  // FOUR_SPACE_SEARCH
