//! @file
//! @brief 緩和四ノビの定義／管理を行うクラス
//! @author Koichi NABETANI
//! @date 2017/08/17

#ifndef RELAXED_FOUR_H
#define RELAXED_FOUR_H

#include <cstdint>

#include "Move.h"

namespace realcore
{
typedef std::uint64_t RelaxedFourID;    //!< Relaxed Four(獲得路、損失路、開残路リスト)を一意に識別するID
static constexpr RelaxedFourID kInvalidFourID = 0;    // 無効なRelaxed Four IDを表す

//! Relaxed Fourのデータ
class RelaxedFour
{
  friend class RelaxedFourTest;

public:
  //! コンストラクタ
  //! @param gain 獲得路
  //! @param cost 損失路
  //! @param rest_list 開残路に着手する緩和四ノビのID
  //! @param four_space_list 実現手順のリスト
  RelaxedFour(const MovePosition gain, const MovePosition cost, const std::vector<MovePosition> &rest_list);

  //! コピーコンストラクタ
  RelaxedFour(const RelaxedFour &relaxed_four);

  //! @brief 代入演算子
  const RelaxedFour& operator=(const RelaxedFour &relaxed_four);

  //! @brief 比較演算子
  const bool operator==(const RelaxedFour &relaxed_four) const;
  const bool operator!=(const RelaxedFour &relaxed_four) const;

  //! 獲得路を取得する
  const MovePosition GetGainPosition() const{
    return gain_;
  }

  //! 損失路を取得する
  const MovePosition GetCostPosition() const{
    return cost_;
  }

  //! 開残路の位置を取得する
  const std::vector<MovePosition>& GetRestPositionList() const{
    return rest_list_;
  }

  //! Relaxed Fourのキーを返す
  const std::uint64_t GetKey() const;

private:
  MovePosition gain_;            //!< 獲得路
  MovePosition cost_;            //!< 損失路
  std::vector<MovePosition> rest_list_;  //!< 開残路リスト
};

}   // namespace realcore
#endif // RELAXED_FOUR_H
