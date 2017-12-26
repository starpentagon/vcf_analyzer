//! @file
//! @brief 開残路の依存関係を管理するクラス
//! @author Koichi NABETANI
//! @date 2017/12/21

#ifndef OPEN_REST_DEPENDENCY_H
#define OPEN_REST_DEPENDENCY_H

#include <map>
#include <set>
#include "OpenRestList.h"

namespace realcore
{

typedef std::map<OpenRestListKey, std::set<OpenRestListKey>> OpenRestDependencyTree;

class OpenRestDependencyTest;

class OpenRestDependency
{
  friend class OpenRestDependencyTest;

public:
  OpenRestDependency();

  //! @brief 開残路キーを追加する
  void Add(const OpenRestListKey open_rest_list_key);

  //! @brief 指定した開残路キーの子のsetを返す
  const std::set<OpenRestListKey>& GetChildSet(const OpenRestListKey open_rest_list_key) const;

  //! @brief 指定した開残路キーに依存するすべてのキーを取得する
  void GetAllDependentKeys(const OpenRestListKey open_rest_list_key, std::set<OpenRestListKey> * const open_rest_key_set) const;

private:
  OpenRestDependencyTree dependency_tree_;    // 「開残路キー」 -> 「開残路キーの子のset」を返すmap
};    // class OpenRestDependency

}   // namespace realcore

#endif    // OPEN_REST_DEPENDENCY_H
