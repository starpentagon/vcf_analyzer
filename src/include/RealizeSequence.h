//! @file
//! @brief 緩和四ノビの実現手順を管理するクラス
//! @author Koichi NABETANI
//! @date 2017/10/28

#ifndef REALIZE_SEQUENCE_H
#define REALIZE_SEQUENCE_H

#include <map>
#include "Move.h"

namespace realcore
{
class RealizeSequence;

//! @brief 2つの実現手順を比較する
//! @retval true 2つの空点状態が同一の内容を保持
bool IsEqual(const RealizeSequence &lhs, const RealizeSequence &rhs);

//! 実現手順のデータ
class RealizeSequence
{
  friend class RealizeSequenceTest;
  friend bool IsEqual(const RealizeSequence &lhs, const RealizeSequence &rhs);

public:
  RealizeSequence();
  
  //! 手順を追加する
  void Add(const MovePosition gain, const MovePosition cost);
  void Add(const RealizeSequence &realize_sequence);
  
  //! 手順が競合するかチェックする
  const bool IsConflict(const MovePosition gain, const MovePosition cost) const;
  const bool IsConflict(const RealizeSequence &realize_sequence) const;

  //! @brief 代入演算子
  const RealizeSequence& operator=(const RealizeSequence &realize_sequence);
  
  //! @brief 比較演算子
  const bool operator==(const RealizeSequence &realize_sequence) const;
  const bool operator!=(const RealizeSequence &realize_sequence) const;

  //! 獲得路を取得する
  const MoveBitSet& GetGainBit() const;

  //! 損失路を取得する
  const MoveBitSet& GetCostBit() const;

  //! 獲得路 -> 損失路のmapを取得する
  const std::map<MovePosition, MovePosition>& GetGainCostMap() const;

private:
  MoveBitSet gain_bit_;   //! 獲得路のbit
  MoveBitSet cost_bit_;   //! 損失路のbit
  std::map<MovePosition, MovePosition> gain_cost_map_;    //! 獲得路 -> 損失路のmap
};

} // namespace realcore
#endif  // REALIZE_SEQUENCE_H
