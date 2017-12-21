//! @file
//! @brief 四ノリ情報を管理するクラス
//! @author Koichi NABETANI
//! @date 2017/12/20

#ifndef OPPONENT_FOUR_INFO_H
#define OPPONENT_FOUR_INFO_H

#include <vector>
#include "Move.h"

namespace realcore{

class OpponentFourInfoTest;

//! 四ノリ情報
class OpponentFourInfo
{
  friend class OpponentFourInfoTest;

public:
  OpponentFourInfo();

  //! @brief 四ノリが発生
  const bool IsOpponentFour() const;

  //! @brief 四ノリ情報を設定する
  //! @pre 設定時は四ノリ情報が未設定であること（四ノリ情報の上書きを行わないこと）
  void SetOpponentFour(const MovePair &opponent_four);
  void SetOpponentFour(const MovePair &opponent_four, const MovePosition disabling_move_1, const MovePosition disabling_move_2);

  //! @brief 四ノリ位置を返す
  //! @pre 四ノリが発生していること
  const MovePosition GetFourPosition() const;

  //! @brief 四ノリ防手位置を返す
  //! @pre 四ノリが発生していること
  const MovePosition GetGuardPosition() const;

  //! @brief 長連筋位置リストを返す
  //! @pre 四ノリが発生していること
  const std::vector<MovePosition>& GetDisablingMoveList() const;

  //! @brief 代入演算子
  const OpponentFourInfo& operator=(const OpponentFourInfo &opponent_four);
  
  //! @brief 比較演算子
  const bool operator==(const OpponentFourInfo &opponent_four) const;
  const bool operator!=(const OpponentFourInfo &opponent_four) const;

private:
  MovePosition four_move_;   //! 四ノリ位置
  MovePosition guard_move_;   //! 四ノリ防手位置
  std::vector<MovePosition> disabling_move_list_;   //! 長連筋位置
};

}   // namespace realcore

#endif  // OPPONENT_FOUR_INFO_H
