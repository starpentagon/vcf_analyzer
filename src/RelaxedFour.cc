#include <functional>

#include "FourSpace.h"
#include "RelaxedFour.h"

using namespace std;

namespace realcore{

RelaxedFour::RelaxedFour(const MovePosition gain, const MovePosition cost, const vector<MovePosition> &rest_move_list)
: gain_(gain), cost_(cost), open_rest_list_(rest_move_list)
{
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
    open_rest_list_ = relaxed_four.GetOpenRestList();
    transposition_table_ = relaxed_four.GetTranspositionTable();
  }

  return *this;
}

const bool RelaxedFour::operator==(const RelaxedFour &relaxed_four) const
{
  bool is_same = gain_ == relaxed_four.GetGainPosition();
  is_same &= cost_ == relaxed_four.GetCostPosition();
  is_same &= open_rest_list_ == relaxed_four.GetOpenRestList();

  return is_same;
}

const bool RelaxedFour::operator!=(const RelaxedFour &relaxed_four) const
{
  return !(*this == relaxed_four);
}

const uint64_t RelaxedFour::GetKey() const
{
  uint64_t key = open_rest_list_.GetOpenRestKey();
  key |= static_cast<uint64_t>(gain_) << 32;
  key |= static_cast<uint64_t>(cost_) << 24;

  return key;
}

const RelaxedFourStatus RelaxedFour::CheckTranspositionTable(const FourSpace &local_four_space) const
{
  for(const auto& element : transposition_table_){
    const auto& four_space = element.first;

    if(four_space == local_four_space){
      return element.second;
    }
  }

  return kRelaxedFourUnknown;
}

const bool RelaxedFour::IsExpandable(const RelaxedFourStatus status) const
{
  assert(status != kRelaxedFourUnknown);
  
  bool is_expandable = status == kRelaxedFourDblFourThree;
  is_expandable |= status == kRelaxedFourOpponentFour;
  is_expandable |= status == kRelaxedFourFeasible;
  
  return is_expandable;
}

}   // namespace realcore