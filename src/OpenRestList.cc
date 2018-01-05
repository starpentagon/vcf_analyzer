#include "MoveList.h"
#include "OpenRestList.h"

using namespace std;

namespace realcore
{

OpenRestList::OpenRestList()
{
  static constexpr size_t kMaxOpenRestSize = 3;
  open_rest_list_.reserve(kMaxOpenRestSize);
}

OpenRestList::OpenRestList(const vector<MovePosition> &rest_move_list)
{
  open_rest_list_ = rest_move_list;
  sort(open_rest_list_.begin(), open_rest_list_.end());
}

OpenRestList::OpenRestList(const OpenRestList &open_rest_list)
{
  *this = open_rest_list;
}

void OpenRestList::Add(const MovePosition rest_move)
{
  open_rest_list_.emplace_back(rest_move);
  sort(open_rest_list_.begin(), open_rest_list_.end());
}

const bool OpenRestList::empty() const
{
  return open_rest_list_.empty();
}

const OpenRestListKey OpenRestList::GetOpenRestKey() const
{
  assert(IsConsistent());

  OpenRestListKey rest_key = 0ULL;

  for(const auto rest_move : open_rest_list_){
    rest_key = rest_key << 8;
    rest_key |= rest_move;
  }

  return rest_key;
}

const vector<MovePosition>& OpenRestList::GetOpenRestMoveList() const
{
  return open_rest_list_;
}

const OpenRestList& OpenRestList::operator=(const OpenRestList &open_rest_list)
{
  if(this != &open_rest_list){
    open_rest_list_ = open_rest_list.GetOpenRestMoveList();
  }

  return *this;
}

const bool OpenRestList::operator==(const OpenRestList &open_rest_list) const
{
  return open_rest_list_ == open_rest_list.GetOpenRestMoveList();
}

const bool OpenRestList::operator!=(const OpenRestList &open_rest_list) const
{
  return !(open_rest_list_ == open_rest_list.GetOpenRestMoveList());
}

const bool OpenRestList::IsConsistent() const
{
  const auto size = open_rest_list_.size();

  for(size_t i=1; i<size; i++){
    const auto prev_value = open_rest_list_[i - 1];
    const auto value = open_rest_list_[i];

    if(!(prev_value < value)){
      return false;
    }
  }

  return true;
}

void GetOpenRestBit(OpenRestListKey open_rest_key, MoveBitSet * const move_bit)
{
  assert(move_bit != nullptr);
  assert(move_bit->none());

  vector<MovePosition> rest_move_list;
  GetOpenRestMoveList(open_rest_key, &rest_move_list);

  for(const auto move : rest_move_list){
    move_bit->set(move);
  }
}

void GetOpenRestMoveList(OpenRestListKey open_rest_key, std::vector<MovePosition> * const rest_move_list)
{
  assert(rest_move_list != nullptr);
  assert(rest_move_list->empty());

  static constexpr OpenRestListKey kMoveMask = 0xFF;
  
  while(open_rest_key != 0){
    const auto rest_move = static_cast<MovePosition>(open_rest_key & kMoveMask);
    open_rest_key = open_rest_key >> 8;

    rest_move_list->emplace_back(rest_move);
  }
}

const MovePosition GetAdditionalMove(const MoveBitSet &super_bit, const MoveBitSet &sub_bit)
{
  assert((super_bit & sub_bit) == sub_bit);
  assert((super_bit | sub_bit) == super_bit);
  
  MoveBitSet diff_bit = super_bit ^ sub_bit;
  
  MoveList additional_move_list;
  GetMoveList(diff_bit, &additional_move_list);
  assert(additional_move_list.size() == 1);

  return additional_move_list[0];
}

std::string GetOpenRestKeyString(const OpenRestListKey open_rest_key)
{
  vector<MovePosition> rest_move_list;
  GetOpenRestMoveList(open_rest_key, &rest_move_list);

  string open_rest_str("(");
  bool is_first = true;

  for(const auto rest_move : rest_move_list){
    if(is_first){
      is_first = false;
    }else{
      open_rest_str += ", ";
    }

    open_rest_str += MoveString(rest_move);
  }

  open_rest_str += ")";

  return open_rest_str;
}

const OpenRestListKey GetParentOpenRestListKey(const MovePosition parent_move, const OpenRestListKey open_rest_list_key)
{
  vector<MovePosition> rest_move_list;
  GetOpenRestMoveList(open_rest_list_key, &rest_move_list);

  const auto erase_it = find(rest_move_list.begin(), rest_move_list.end(), parent_move);
  assert(erase_it != rest_move_list.end());

  rest_move_list.erase(erase_it);
  OpenRestList parent_open_rest(rest_move_list);

  return parent_open_rest.GetOpenRestKey();
}

const bool IsSingleRestMove(const OpenRestListKey key)
{
  return (key <= kMoveNum);
}


}   // namespace realcore
