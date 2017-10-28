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

namespace realcore{

//! 到達領域を保持するテーブル
typedef std::array<std::vector<RelaxedFourID>, kMoveNum> ReachRegion;

//! 設置領域を保持するテーブル
typedef std::array<std::vector<RelaxedFourID>, kMoveNum> PutRegion;

//! 獲得路から新たな緩和四ノビする際の情報を保持するtuple
//! @note 1要素目: gain
//! @note 2要素目: cost
//! @note 3要素目: rest_max(開残路がない場合はkNullMove)
//! @note 4要素目: rest_min(開残路がない場合はkNullMove)
typedef std::tuple<MovePosition, MovePosition, MovePosition, MovePosition> NextRelaxedFourInfo;

//! 開残路へ到達可能な到達路のRelaxedFourID
typedef std::pair<RelaxedFourID, RelaxedFourID> RestGainFourID;

class FourSpaceSearchTest;

class FourSpaceSearch
: public BitBoard
{
  friend class FourSpaceSearchTest;

public:
  FourSpaceSearch(const BitBoard &bit_board);

  //! 四追い空間を展開する
  //! @note 初期局面での相手の四ノビは考慮しない
  template<PlayerTurn P>
  void ExpandFourSpace();
  void ExpandFourSpace(const bool is_black_turn);

  //! 四追い空間を展開する(四ノビ指定版)
  template<PlayerTurn P>
  void ExpandFourSpace(const std::vector<MovePair> &four_list);
  void ExpandFourSpace(const bool is_black_turn, const std::vector<MovePair> &four_list);

  //! 緩和四ノビの数
  const size_t GetRelaxedFourCount() const;

  //! 緩和四ノビの最長回数
  //! @note 解禁や四ノリを防ぐためにVCF手数は緩和四ノビ最長回数を上回る可能性あり
  const size_t GetMaxRelaxedFourLength() const;

private:
  //! Relaxed Fourのデータを追加する
  const RelaxedFourID AddRelaxedFour(const RelaxedFour &relaxed_four);
  const RelaxedFourID AddRelaxedFour(const MovePosition gain_position, const MovePosition cost_position, const std::vector<MovePosition> &rest_list);

  //! 到達領域、設置領域を更新する
  //! @param relaxed_four_id ID
  //! @param gain_bit 着手済みの獲得路
  //! @param cost_bit 着手済みの損失路
  template<PlayerTurn P>
  void UpdateReachPutRegion(const RelaxedFourID relaxed_four_id);

  //! 到達路1つ + 自石2つでRelaxed Fourを作る手を列挙する
  template<PlayerTurn P>
  void GetRelaxedFourFromOneGainPosition(const BoardOpenState &board_open_state, std::vector<NextRelaxedFourInfo> * const next_four_info_list) const;

  //! 到達路2つ + 自石1つでRelaxed Fourを作る手を列挙する
  //! @param additional_gain 追加で着手した到達路(到達路3つからRelaxed Four生成用)
  template<PlayerTurn P>
  void GetRelaxedFourFromTwoGainPosition(const BoardOpenState &board_open_state, const MovePosition additional_gain, std::vector<NextRelaxedFourInfo> * const next_four_info_list) const;

  //! 到達路3つでRelaxed Fourを作る手を列挙する
  template<PlayerTurn P>
  void GetRelaxedFourFromThreeGainPosition(const MovePosition gain_position, std::vector<NextRelaxedFourInfo> * const next_four_info_list);

  //! 指定位置の特定方向の近傍(開残路位置)に存在する獲得路を求める
  //! @param gain_position 指定位置
  //! @param direction 方向
  //! @param restable_four_id_list 開残路位置にあるRelaxed FourID
  //! @retval 開残路位置の数
  size_t GetRestableRelaxedFourIDList(const MovePosition gain_position, const BoardDirection direction, std::vector<RelaxedFourID> * const restable_four_id_list) const;

  //! 開残路へ到達可能な獲得路ペアのリストを取得する
  void GetRestRelaxedFourID(const NextRelaxedFourInfo &next_four_info, std::vector<RestGainFourID> * const rest_gain_id_list) const;

  //! Relaxed FourIDの到達路を求める
  //! @retval true 到達路を実現可能, false 競合しており実現不可能
  bool GetReachSequence(const RelaxedFourID relaxed_four_id, MoveList * const move_list) const;
  bool GetReachSequence(const std::vector<RelaxedFourID> &four_id_list, MoveList * const move_list) const;

  //! Relaxed FourIDの到達路(Relaxed FourIDリスト)を求める
  void GetReachIDSequence(const RelaxedFourID relaxed_four_id, std::vector<RelaxedFourID> * const id_list) const;
  void GetReachIDSequence(const RelaxedFourID relaxed_four_id, std::set<RelaxedFourID> * const appeared_id_set, std::vector<RelaxedFourID> * const id_list) const;

  //! 末端ノード(どの緩和四ノビの残路になっていない緩和四ノビ)を列挙する
  void EnumerateLeaf(std::vector<RelaxedFourID> * const leaf_id_list) const;

  template<PositionState>
  void SetState(const MovePosition move);

  //! 獲得路の実現に必要な依存路の着手(獲得路/損失路)を求める
  //! @note 依存路の探索は深さ１に限定
  void GetDependentReachMove(const RelaxedFourID relaxed_four_id, std::vector<std::vector<MovePair>> * const move_pair_vector) const;

  //! 獲得路に対応する損失路のリストを返す
  //! @param gain_move 獲得路
  //! @param const_move_list 損失路のリスト
  void GetCostMoveList(const MovePosition gain_move, MoveList * const cost_move_list) const;

  //! 展開済みの変化かチェックする
  const bool IsExpanded(const RelaxedFourID relaxed_four_id) const;

  std::vector<RelaxedFour> relaxed_four_list_;   //! relaxed_four_list_[RelaxedFourID] -> RelaxedFourIDに対応するRelaxedFourのデータ
  std::map<std::uint64_t, RelaxedFourID> relaxed_four_transposition_map_;    //! relaxed_fourの置換表
  std::array< std::set<std::uint64_t>, kMoveNum> expanded_relaxed_four_;    //! 獲得路ごとのすでに展開済みのパターン(獲得路、損失路、依存路の獲得路/損失路)
  
  ReachRegion reach_region_;    //! 到達領域
  PutRegion put_region_;        //! 設置領域

  PlayerTurn attack_player_;    //! 詰め方(黒 or 白)
  MoveList search_sequence_;    //! 探索手順
  std::set<HashValue> transposition_set_;   //! 置換表
};

}   // namespace realcore

#include "FourSpaceSearch-inl.h"

#endif  // FOUR_SPACE_SEARCH
