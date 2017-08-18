#include "RelaxedFour.h"

using namespace std;

namespace realcore{

RelaxedFour::RelaxedFour(const MovePosition gain, const MovePosition cost, const std::vector<RelaxedFourID> &rest_list)
: gain_(gain), cost_(cost), rest_list_(rest_list)
{
  sort(rest_list_.begin(), rest_list_.end());
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

}   // namespace realcore