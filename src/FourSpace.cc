#include "FourSpace.h"
#include "MoveList.h"

using namespace std;

namespace realcore
{

FourSpace::FourSpace()
{
}

FourSpace::FourSpace(const MovePosition gain_position, const MovePosition cost_position)
{
  Add(gain_position, cost_position);
}

FourSpace::FourSpace(const FourSpace &four_space, const MoveBitSet &neighborhood_bit)
{
  gain_bit_ = four_space.GetGainBit() & neighborhood_bit;
  cost_bit_ = four_space.GetCostBit() & neighborhood_bit;
}

void FourSpace::Add(const MovePosition gain_position, const MovePosition cost_position)
{
  assert(!IsConflict(gain_position, cost_position));

  gain_bit_.set(gain_position);
  cost_bit_.set(cost_position);
}

void FourSpace::Add(const FourSpace &four_space)
{
  assert(IsPuttable(four_space));
  
  gain_bit_ |= four_space.GetGainBit();
  cost_bit_ |= four_space.GetCostBit();
}

const bool FourSpace::IsConflict(const MovePosition gain_position, const MovePosition cost_position) const
{
  if(gain_bit_[gain_position] || gain_bit_[cost_position] || cost_bit_[gain_position] || cost_bit_[cost_position]){
    return true;
  }

  return false;
}

const bool FourSpace::IsPuttable() const
{
  return (gain_bit_ & cost_bit_).none();
}

const bool FourSpace::IsPuttable(const FourSpace &four_space) const
{
  assert(four_space.IsPuttable());

  const auto &check_gain_bit = four_space.GetGainBit();
  const auto &check_cost_bit = four_space.GetCostBit();

  if((check_gain_bit & cost_bit_).any()){
    return false;
  }

  if((check_cost_bit & gain_bit_).any()){
    return false;
  }

  return true;
}

const MoveBitSet& FourSpace::GetGainBit() const
{
  return gain_bit_;
}

const MoveBitSet FourSpace::GetNeighborhoodGainBit(const MoveBitSet &neighborhood_bit) const
{
  return gain_bit_ & neighborhood_bit;
}

const MoveBitSet& FourSpace::GetCostBit() const
{
  return cost_bit_;
}

const MoveBitSet FourSpace::GetNeighborhoodCostBit(const MoveBitSet &neighborhood_bit) const
{
  return cost_bit_ & neighborhood_bit;
}

const FourSpace& FourSpace::operator=(const FourSpace &four_space)
{
  if(this != &four_space){
    gain_bit_ = four_space.GetGainBit();
    cost_bit_ = four_space.GetCostBit();
  }

  return *this;
}

const bool FourSpace::operator==(const FourSpace &four_space) const
{
  if(gain_bit_ != four_space.GetGainBit()){
    return false;
  }

  if(cost_bit_ != four_space.GetCostBit()){
    return false;
  }

  return true;
}

const bool FourSpace::operator!=(const FourSpace &four_space) const
{
  return !(*this == four_space);
}

}

