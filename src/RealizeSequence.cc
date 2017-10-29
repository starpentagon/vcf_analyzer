#include "RealizeSequence.h"
#include "MoveList.h"

using namespace std;

namespace realcore
{

RealizeSequence::RealizeSequence()
{
}

void RealizeSequence::Add(const MovePosition gain, const MovePosition cost)
{
  assert(!IsConflict(gain, cost));

  gain_bit_.set(gain);
  cost_bit_.set(cost);

  gain_cost_map_.insert(make_pair(gain, cost));
}

void RealizeSequence::Add(const RealizeSequence &realize_sequence)
{
  assert(!IsConflict(realize_sequence));
  
  gain_bit_ |= realize_sequence.GetGainBit();
  cost_bit_ |= realize_sequence.GetCostBit();

  const auto &map = realize_sequence.GetGainCostMap();
  gain_cost_map_.insert(map.begin(), map.end());
}

const bool RealizeSequence::IsConflict(const MovePosition gain, const MovePosition cost) const
{
  const auto find_it = gain_cost_map_.find(gain);
  
  if(find_it != gain_cost_map_.end()){
    return find_it->second != cost;
  }

  // gain_bit_[gain] = falseが保証される
  return cost_bit_[gain] || gain_bit_[cost] || cost_bit_[cost];
}

const bool RealizeSequence::IsConflict(const RealizeSequence &realize_sequence) const
{
  auto check_gain_bit = realize_sequence.GetGainBit();
  auto check_cost_bit = realize_sequence.GetCostBit();
  const auto &check_gain_move_map = realize_sequence.GetGainCostMap();
  
  const auto same_gain_bit = gain_bit_ & check_gain_bit;

  MoveList same_gain_move;
  GetMoveList(same_gain_bit, &same_gain_move);

  for(const auto gain_move : same_gain_move){
    const auto cost_move = gain_cost_map_.find(gain_move)->second;
    const auto check_cost_move = check_gain_move_map.find(gain_move)->second;

    if(cost_move != check_cost_move){
      return true;
    }

    check_gain_bit.reset(gain_move);
    check_cost_bit.reset(cost_move);
  }

  // gain_bit_とcheck_gain_bitの重複がないことが保証される
  check_gain_bit |= gain_bit_;

  if((check_gain_bit & cost_bit_).any()){
    return true;
  }

  check_gain_bit |= cost_bit_;

  if((check_gain_bit & check_cost_bit).any()){
    return true;
  }
  
  return false;
}

const MoveBitSet& RealizeSequence::GetGainBit() const
{
  return gain_bit_;
}

const MoveBitSet& RealizeSequence::GetCostBit() const
{
  return cost_bit_;
}

const std::map<MovePosition, MovePosition>& RealizeSequence::GetGainCostMap() const
{
  return gain_cost_map_;
}

const RealizeSequence& RealizeSequence::operator=(const RealizeSequence &realize_sequence)
{
  if(this != &realize_sequence){
    gain_bit_ = realize_sequence.GetGainBit();
    cost_bit_ = realize_sequence.GetCostBit();
    gain_cost_map_ = realize_sequence.GetGainCostMap();
  }

  return *this;
}

const bool RealizeSequence::operator==(const RealizeSequence &realize_sequence) const
{
  if(gain_bit_ != realize_sequence.GetGainBit()){
    return false;
  }

  if(cost_bit_ != realize_sequence.GetCostBit()){
    return false;
  }

  if(gain_cost_map_ != realize_sequence.GetGainCostMap()){
    return false;
  }

  return true;
}

const bool RealizeSequence::operator!=(const RealizeSequence &realize_sequence) const
{
  return !(*this == realize_sequence);
}

}

