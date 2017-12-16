//! @file
//! @brief 緩和四ノビの定義／管理を行うクラス
//! @author Koichi NABETANI
//! @date 2017/08/17

#ifndef RELAXED_FOUR_H
#define RELAXED_FOUR_H

#include <cstdint>

#include "Move.h"
#include "BitBoard.h"

namespace realcore
{

  //! @brief R-四ノビ状態
enum RelaxedFourStatus{
  kRelaxedFourUnknown,       //! 不明
  kRelaxedFourInfeasible,    //! 実現不可能
  kRelaxedFourFail,          //! R-四ノビできず
  kRelaxedFourDblFourThree,  //! 四々/三々
  kRelaxedFourTerminate,     //! 終端
  kRelaxedFourOpponentFour,  //! 四ノリ
  kRelaxedFourFeasible       //! R-四ノビが可能
};

typedef std::uint64_t RelaxedFourID;    //!< Relaxed Four(獲得路、損失路、開残路リスト)を一意に識別するID
static constexpr RelaxedFourID kInvalidFourID = 0;    // 無効なRelaxed Four IDを表す

class FourSpace;

typedef std::vector<std::pair<FourSpace, RelaxedFourStatus>> RelaxedFourStatusTable;    //! 置換表

//! Relaxed Fourのデータ
class RelaxedFour
{
  friend class RelaxedFourTest;

public:
  //! @brief コンストラクタ
  //! @param gain 獲得路
  //! @param cost 損失路
  //! @param rest_list 開残路に着手する緩和四ノビのID
  //! @param four_space_list 実現手順のリスト
  RelaxedFour(const MovePosition gain, const MovePosition cost, const std::vector<MovePosition> &rest_list);

  //! @brief コピーコンストラクタ
  RelaxedFour(const RelaxedFour &relaxed_four);

  //! @brief 代入演算子
  const RelaxedFour& operator=(const RelaxedFour &relaxed_four);

  //! @brief 比較演算子
  const bool operator==(const RelaxedFour &relaxed_four) const;
  const bool operator!=(const RelaxedFour &relaxed_four) const;

  //! @brief 獲得路を取得する
  const MovePosition GetGainPosition() const;

  //! @brief 損失路を取得する
  const MovePosition GetCostPosition() const;

  //! @brief 開残路の位置を取得する
  const std::vector<MovePosition>& GetRestPositionList() const;

  //! @brief 置換表を取得する
  const RelaxedFourStatusTable& GetTranspositionTable() const;

  //! @brief Relaxed Fourのキーを返す
  //! @note (獲得路, 損失路, 開残路リスト)を一意に識別するキー
  const std::uint64_t GetKey() const;

  //! @brief 獲得/損失空間に対するR-四ノビ状態を取得する
  //! @param four_space 開残路の獲得/損失空間
  //! @param 開始局面のBitBoard
  //! @param 四ノリがある場合の四ノリする手とその防手の格納先
  //! @pre 開残路の獲得/損失空間で五連以上ができていないこと
  template<PlayerTurn P>
  const RelaxedFourStatus GetRelaxedFourStatus(const FourSpace &four_space, const BitBoard &bit_board, MovePair * const opponent_four);

  //! @brief 登録済みの獲得/損失空間かチェックする
  //! @retval 未登録の場合はkRelaxedFourUnknownを返し、登録済みの場合はR-四ノビ状態を返す
  const RelaxedFourStatus CheckTranspositionTable(const FourSpace &local_four_space) const;

  //! @brief 展開可能なR-四ノビ状態かチェックする
  const bool IsExpandable(const RelaxedFourStatus status) const;
  
private:
  template<PlayerTurn P>
  const bool CanGuardOpponentFour(const FourSpace &four_space, const MovePair &move_pair, const BitBoard &bit_board) const;
  
  MovePosition gain_;            //!< 獲得路
  MovePosition cost_;            //!< 損失路
  std::vector<MovePosition> rest_list_;  //!< 開残路リスト
  RelaxedFourStatusTable transposition_table_;    //! LocalFourSpace -> RelaxedFourStatusの置換表
};

}   // namespace realcore

#include "RelaxedFour-inl.h"

#endif // RELAXED_FOUR_H
