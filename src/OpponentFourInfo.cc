#include "OpponentFourInfo.h"

using namespace std;
using namespace realcore;

OpponentFourInfo::OpponentFourInfo()
: four_move_(kNullMove), guard_move_(kNullMove)
{
  disabling_move_list_.reserve(2);
}

const bool OpponentFourInfo::IsOpponentFour() const
{
  return four_move_ != kNullMove;
}

const MovePosition OpponentFourInfo::GetFourPosition() const
{
  assert(IsOpponentFour());
  return four_move_;
}

const MovePosition OpponentFourInfo::GetGuardPosition() const
{
  assert(IsOpponentFour());
  return guard_move_;
}

const vector<MovePosition>& OpponentFourInfo::GetDisablingMoveList() const
{
  assert(IsOpponentFour());
  return disabling_move_list_;
}

void OpponentFourInfo::SetOpponentFour(const MovePair &opponent_four, const MovePosition disabling_move_1, const MovePosition disabling_move_2)
{
  assert(!IsOpponentFour());
  four_move_ = opponent_four.first;
  guard_move_ = opponent_four.second;

  if(disabling_move_1 != kNullMove){
    disabling_move_list_.emplace_back(disabling_move_1);
  }

  if(disabling_move_2 != kNullMove){
    disabling_move_list_.emplace_back(disabling_move_2);
  }
}

void OpponentFourInfo::SetOpponentFour(const MovePair &opponent_four)
{
  SetOpponentFour(opponent_four, kNullMove, kNullMove);
}

const OpponentFourInfo& OpponentFourInfo::operator=(const OpponentFourInfo &opponent_four)
{
  if(this != &opponent_four){
    if(opponent_four.IsOpponentFour()){
      four_move_ = opponent_four.GetFourPosition();
      guard_move_ = opponent_four.GetGuardPosition();
      disabling_move_list_ = opponent_four.GetDisablingMoveList();
    }else{
      four_move_ = kNullMove;
      guard_move_ = kNullMove;
      disabling_move_list_.clear();
    }
  }

  return *this;
}

const bool OpponentFourInfo::operator==(const OpponentFourInfo &opponent_four) const
{
  if(!opponent_four.IsOpponentFour()){
    return !IsOpponentFour();
  }

  if(four_move_ != opponent_four.GetFourPosition()){
    return false;
  }

  if(guard_move_ != opponent_four.GetGuardPosition()){
    return false;
  }

  if(disabling_move_list_ != opponent_four.GetDisablingMoveList()){
    return false;
  }

  return true;
}

const bool OpponentFourInfo::operator!=(const OpponentFourInfo &opponent_four) const
{
  return !(*this == opponent_four);
}
