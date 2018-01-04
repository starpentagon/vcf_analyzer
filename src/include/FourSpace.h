//! @file
//! @brief 獲得/損失空間を保持するクラス
//! @author Koichi NABETANI
//! @date 2017/10/28

#ifndef FOUR_SPACE_H
#define FOUR_SPACE_H

#include "Move.h"
#include "OpponentFourInfo.h"

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
  
  //! @brief 手順を追加する
  //! @note 四ノリ情報は別途追加する必要あり
  void Add(const MovePosition gain_position, const MovePosition cost_position);
  void Add(const FourSpace &four_space);

  //! @brief 指し手が競合していないかチェックする
  const bool IsConflict(const MovePosition gain_position, const MovePosition cost_position) const;

  //! @brief 設置可能かチェックする
  const bool IsPuttable() const;

  //! @brief 他の獲得/損失空間と同時設置可能かチェックする
  const bool IsPuttable(const FourSpace &four_space) const;

  //! @brief 均等(Gain, Cost数が一致する)かチェックする
  const bool IsBalanced() const;

  //! @brief 代入演算子
  const FourSpace& operator=(const FourSpace &four_space);
  
  //! @brief 比較演算子
  const bool operator==(const FourSpace &four_space) const;
  const bool operator!=(const FourSpace &four_space) const;

  //! @brief 獲得路を取得する
  const MoveBitSet& GetGainBit() const;

  //! @brief 獲得路を取得する
  //! @param neighborhood_bit 直線近傍のbit
  const MoveBitSet GetNeighborhoodGainBit(const MoveBitSet &neighborhood_bit) const;

  //! @brief 損失路を取得する
  const MoveBitSet& GetCostBit() const;

  //! @brief 損失路を取得する
  //! @param neighborhood_bit 直線近傍のbit
  const MoveBitSet GetNeighborhoodCostBit(const MoveBitSet &neighborhood_bit) const;

  //! @brief 四ノリ情報を取得する
  const OpponentFourInfo& GetOpponentFourInfo() const;

  //! @brief Hash値を計算する
  const size_t CalcHashValue() const;

private:
  MoveBitSet gain_bit_;   //! 獲得路のbit
  MoveBitSet cost_bit_;   //! 損失路のbit
};

} // namespace realcore
#endif  // FOUR_SPACE_H
