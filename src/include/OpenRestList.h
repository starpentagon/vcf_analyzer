//! @file
//! @brief 開残路を管理するクラス
//! @author Koichi NABETANI
//! @date 2017/12/21

#ifndef OPEN_REST_LIST_H
#define OPEN_REST_LIST_H

#include <vector>
#include "Move.h"

namespace realcore
{

//! 開残路リストのkey
typedef std::uint64_t OpenRestListKey;

//! @brief 開残路リストのkeyからMoveBitSetを生成する
void GetOpenRestBit(OpenRestListKey open_rest_key, MoveBitSet * const move_bit);
void GetOpenRestMoveList(OpenRestListKey open_rest_key, std::vector<MovePosition> * const rest_move_list);

//! @brief 親の開残路キーを取得する
const OpenRestListKey GetParentOpenRestListKey(const MovePosition parent_move, const OpenRestListKey open_rest_list_key);

//! @brief MoveBitSetの差分位置を取得する
//! @pre super_bitはsub_bitを包含していること
//! @pre super_bitとsub_bitの差分は１つであること
const MovePosition GetAdditionalMove(const MoveBitSet &super_bit, const MoveBitSet &sub_bit);

//! @brief 開残路キーの文字列を取得する
std::string GetOpenRestKeyString(const OpenRestListKey open_rest_key);

class OpenRestListTest;

class OpenRestList
{
  friend class OpenRestListTest;

public:
  OpenRestList();
  OpenRestList(const std::vector<MovePosition> &rest_move_list);
  OpenRestList(const OpenRestList &open_rest_list);

  //! @brief 開残路を追加する
  void Add(const MovePosition rest_move);

  //! @brief 開残路が空か返す
  const bool empty() const;

  //! @brief 開残路リストキーを取得する
  const OpenRestListKey GetOpenRestKey() const;
  
  //! @brief 開残路リストを取得する
  const std::vector<MovePosition>& GetOpenRestMoveList() const;

  //! @brief 代入演算子
  const OpenRestList& operator=(const OpenRestList &open_rest_list);
  
  //! @brief 比較演算子
  const bool operator==(const OpenRestList &open_rest_list) const;
  const bool operator!=(const OpenRestList &open_rest_list) const;

private:
  //! @brief 開残路位置が降順ソートされているかチェックする
  //! @retval true 開残路位置が降順ソートされている
  const bool IsConsistent() const;

  std::vector<MovePosition> open_rest_list_;    //! 開残路位置を昇順ソートして保持する
};    // class OpenRestList

}   // namespace realcore

#endif  // OPEN_REST_LIST_H