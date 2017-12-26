#include "OpenRestDependency.h"

using namespace std;
using namespace realcore;

OpenRestDependency::OpenRestDependency()
{
}

void OpenRestDependency::Add(const OpenRestListKey open_rest_list_key)
{
  vector<MovePosition> rest_position_list;
  GetOpenRestMoveList(open_rest_list_key, &rest_position_list);
  const auto rest_size = rest_position_list.size();

  if(rest_size <= 1){
    return;
  }
  
  if(rest_size == 2){
    for(const auto rest_move : rest_position_list){
      dependency_tree_[rest_move].insert(open_rest_list_key);
    }
  }else if(rest_size == 3){
    static constexpr std::array<size_t, 3> index_min_list{{0, 0, 1}};
    static constexpr std::array<size_t, 3> index_max_list{{1, 2, 2}};
    
    for(size_t i=0; i<3; i++){
      const auto index_min = index_min_list[i];
      const auto index_max = index_max_list[i];

      const auto rest_min = rest_position_list[index_min];
      const auto rest_max = rest_position_list[index_max];

      std::vector<MovePosition> parent_rest_list{
        rest_min, rest_max
      };

      OpenRestList open_rest_list(parent_rest_list);

      const auto parent_rest_key = open_rest_list.GetOpenRestKey();
      dependency_tree_[parent_rest_key].insert(open_rest_list_key);

      Add(parent_rest_key);
    }
  }  
}

const std::set<OpenRestListKey>& OpenRestDependency::GetChildSet(const OpenRestListKey open_rest_list_key) const
{
  const auto find_it = dependency_tree_.find(open_rest_list_key);

  if(find_it == dependency_tree_.end()){
    static const std::set<OpenRestListKey> empty_set;
    return empty_set;
  }

  return find_it->second;
}

void OpenRestDependency::GetAllDependentKeys(const OpenRestListKey open_rest_list_key, std::set<OpenRestListKey> * const open_rest_key_set) const
{
  assert(open_rest_key_set != nullptr);

  if(open_rest_key_set->find(open_rest_list_key) != open_rest_key_set->end()){
    return;
  }

  const auto& child_open_rest_key_set = GetChildSet(open_rest_list_key);

  for(const auto child_key : child_open_rest_key_set){
    open_rest_key_set->insert(child_key);
    GetAllDependentKeys(child_key, open_rest_key_set);
  }
}
