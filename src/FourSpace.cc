#include <set>

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
  opponent_four_list_ = four_space.GetOpponentFourList();
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

  set<uint64_t> move_pair_key_set;

  for(const auto move_pair : opponent_four_list_){
    const auto four_move = move_pair.first;
    const auto guard_move = move_pair.second;
    const uint64_t key = four_move << 8 | guard_move;

    move_pair_key_set.insert(key);
  }

  const auto& opponet_four_list = four_space.GetOpponentFourList();

  for(const auto move_pair : opponet_four_list){
    const auto four_move = move_pair.first;
    const auto guard_move = move_pair.second;
    const uint64_t key = four_move << 8 | guard_move;

    const auto find_it = move_pair_key_set.find(key);

    if(find_it != move_pair_key_set.end()){
      continue;
    }

    opponent_four_list_.emplace_back(move_pair);
  }
}

void FourSpace::AddOpponentFour(const MovePair &opponent_four)
{
  const auto find_it = find(opponent_four_list_.begin(), opponent_four_list_.end(), opponent_four);

  if(find_it != opponent_four_list_.end()){
    return;
  }

  opponent_four_list_.emplace_back(opponent_four);

  // todo delete -- 
  static size_t size = 0;

  if(size < opponent_four_list_.size()){
    size = opponent_four_list_.size();
    cerr << "opponent size: " << size << endl;
  }
  // -- todo delete
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
  return (gain_bit_ & cost_bit_).none() && (gain_bit_.count() == cost_bit_.count());
}

const bool FourSpace::IsPuttable(const FourSpace &four_space) const
{
  assert(four_space.IsPuttable());

  const auto &check_gain_bit = four_space.GetGainBit();
  const auto &check_cost_bit = four_space.GetCostBit();

  // @note cost bitのチェックを先にした方が3%程度高速
  if((check_cost_bit & gain_bit_).any()){
    return false;
  }

  if((check_gain_bit & cost_bit_).any()){
    return false;
  }

  if((check_gain_bit | gain_bit_).count() != (check_cost_bit | cost_bit_).count()){
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

const vector<MovePair>& FourSpace::GetOpponentFourList() const
{
  return opponent_four_list_;
}

const FourSpace& FourSpace::operator=(const FourSpace &four_space)
{
  if(this != &four_space){
    gain_bit_ = four_space.GetGainBit();
    cost_bit_ = four_space.GetCostBit();
    opponent_four_list_ = four_space.GetOpponentFourList();
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

  if(opponent_four_list_ != four_space.GetOpponentFourList()){
    return false;
  }

  return true;
}

const bool FourSpace::operator!=(const FourSpace &four_space) const
{
  return !(*this == four_space);
}

}

