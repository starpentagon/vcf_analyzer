//! @file
//! @brief Four Space Searchを行うクラス
//! @author Koichi NABETANI
//! @date 2017/08/15
#ifndef FOUR_SPACE_SEARCH_H
#define FOUR_SPACE_SEARCH_H

#include <tuple>
#include <array>
#include <vector>

#include "RelaxedFour.h"
#include "BitBoard.h"

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

private:
  //! Relaxed Fourのデータを追加する
  const RelaxedFourID AddRelaxedFour(const RelaxedFour &relaxed_four);
  const RelaxedFourID AddRelaxedFour(const MovePosition gain_position, const MovePosition cost_position, const std::vector<RelaxedFourID> &rest_list);

  //! 到達領域、設置領域を更新する
  //! @param relaxed_four_id ID
  //! @param gain_bit 着手済みの獲得路
  //! @param cost_bit 着手済みの損失路
  template<PlayerTurn P>
  void UpdateReachPutRegion(const RelaxedFourID relaxed_four_id, MoveBitSet * const gain_bit, MoveBitSet * const cost_bit);

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

  //! 開残路になりうる獲得路を求める
  void GetRestableGainPositionList(const MovePosition gain_position, const BoardDirection direction, std::vector<MovePosition> * const restable_gain_list) const;

  //! 開残路へ到達可能な獲得路ペアのリストを取得する
  void GetRestRelaxedFourID(const NextRelaxedFourInfo &next_four_info, std::vector<RestGainFourID> * const rest_gain_id_list) const;

  //! Relaxed FourIDの到達路bitsetを求める
  void GetReachableBit(const RelaxedFourID relaxed_four_id, MoveBitSet * const gain_bit, MoveBitSet * const cost_bit) const;

  //! 競合しているかチェックする
  const bool IsConflict(const MoveBitSet &gain_bit_1, const MoveBitSet &cost_bit_1, const MoveBitSet &gain_bit_2, const MoveBitSet &cost_bit_2);

  std::vector<RelaxedFour> relaxed_four_list_;   //! relaxed_four_list_[RelaxedFourID] -> RelaxedFourIDに対応するRelaxedFourのデータ
  
  ReachRegion reach_region_;    //! 到達領域
  PutRegion put_region_;        //! 設置領域
};

}   // namespace realcore

#include "FourSpaceSearch-inl.h"

#endif  // FOUR_SPACE_SEARCH
