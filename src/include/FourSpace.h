//! @file
//! @brief 緩和四ノビの獲得/損失空間を管理するクラス
//! @author Koichi NABETANI
//! @date 2017/10/28

#ifndef FOUR_SPACE_H
#define FOUR_SPACE_H

#include <map>
#include "Move.h"

namespace realcore
{

class FourSpaceTest;

//! 獲得/損失空間
class FourSpace
{
  friend class FourSpaceTest;

public:
  FourSpace();
  FourSpace(const MovePosition gain_position, const MovePosition cost_position);
  FourSpace(const FourSpace &four_space, const MoveBitSet &neighborhood_bit);
  
  //! 手順を追加する
  void Add(const MovePosition gain_position, const MovePosition cost_position);
  void Add(const FourSpace &four_space);
  
  //! 指し手が競合していないかチェックする
  const bool IsConflict(const MovePosition gain_position, const MovePosition cost_position) const;

  //! 設置可能かチェックする
  const bool IsPuttable() const;

  //! 他の獲得/損失空間と同時設置可能かチェックする
  const bool IsPuttable(const FourSpace &four_space) const;

  //! @brief 代入演算子
  const FourSpace& operator=(const FourSpace &four_space);
  
  //! @brief 比較演算子
  const bool operator==(const FourSpace &four_space) const;
  const bool operator!=(const FourSpace &four_space) const;

  //! 獲得路を取得する
  const MoveBitSet& GetGainBit() const;

  //! 獲得路を取得する
  //! @param neighborhood_bit 直線近傍のbit
  const MoveBitSet GetNeighborhoodGainBit(const MoveBitSet &neighborhood_bit) const;

  //! 損失路を取得する
  const MoveBitSet& GetCostBit() const;

  //! 損失路を取得する
  //! @param neighborhood_bit 直線近傍のbit
  const MoveBitSet GetNeighborhoodCostBit(const MoveBitSet &neighborhood_bit) const;

private:
  MoveBitSet gain_bit_;   //! 獲得路のbit
  MoveBitSet cost_bit_;   //! 損失路のbit
};

} // namespace realcore
#endif  // FOUR_SPACE_H
