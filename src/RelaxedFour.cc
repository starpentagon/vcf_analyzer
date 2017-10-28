#include "RelaxedFour.h"

using namespace std;

namespace realcore{

RelaxedFour::RelaxedFour(const MovePosition gain, const MovePosition cost, const std::vector<MovePosition> &rest_list)
: gain_(gain), cost_(cost), rest_list_(rest_list)
{
  sort(rest_list_.begin(), rest_list_.end(), greater<MovePosition>());
}

RelaxedFour::RelaxedFour(const RelaxedFour &relaxed_four)
{
  *this = relaxed_four;
}

const RelaxedFour& RelaxedFour::operator=(const RelaxedFour &relaxed_four)
{
  if(this != &relaxed_four){
    gain_ = relaxed_four.GetGainPosition();
    cost_ = relaxed_four.GetCostPosition();
    rest_list_ = relaxed_four.GetRestPositionList();
  }

  return *this;
}

const bool RelaxedFour::operator==(const RelaxedFour &relaxed_four) const
{
  bool is_same = gain_ == relaxed_four.GetGainPosition();
  is_same &= cost_ == relaxed_four.GetCostPosition();
  is_same &= rest_list_ == relaxed_four.GetRestPositionList();

  return is_same;
}

const bool RelaxedFour::operator!=(const RelaxedFour &relaxed_four) const
{
  return !(*this == relaxed_four);
}

const uint64_t RelaxedFour::GetKey() const
{
  uint64_t key = static_cast<uint64_t>(gain_) << 32;
  key |= static_cast<uint64_t>(cost_) << 24;

  const auto rest_size = rest_list_.size();
  assert(rest_size <= 3);

  if(rest_size >= 1){
    key |= static_cast<uint64_t>(rest_list_[0]) << 16;
  }
  
  if(rest_size >= 2){
    assert(rest_list_[0] > rest_list_[1]);
    key |= static_cast<uint64_t>(rest_list_[1]) << 8;    
  }
  
  if(rest_size == 3){
    assert(rest_list_[1] > rest_list_[2]);
    key |= static_cast<uint64_t>(rest_list_[2]);
  }

  return key;
}

}   // namespace realcore