#include "FourSpaceManager.h"

using namespace std;
using namespace realcore;

FourSpaceManager::FourSpaceManager()
{
  // kInvalidFourSpaceIDに対応する要素を追加
  four_space_list_.emplace_back();
}

const FourSpace& FourSpaceManager::GetFourSpace(const FourSpaceID four_space_id) const
{
  assert(four_space_id < four_space_list_.size());
  return four_space_list_[four_space_id];
}

void FourSpaceManager::AddFourSpace(const MovePosition gain_move, const MovePosition cost_move, const FourSpace &four_space, std::vector<RestKeyFourSpace> * const added_four_space_list)
{
  assert(added_four_space_list != nullptr);
  assert(added_four_space_list->empty());
}

const FourSpaceID FourSpaceManager::GetFourSpaceID(const MovePosition move, const FourSpace &four_space) const
{
  return kInvalidFourSpaceID;
}
