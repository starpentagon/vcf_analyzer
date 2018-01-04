#include "gtest/gtest.h"
#include "OpenRestDependency.h"

using namespace std;

namespace realcore
{
  
class OpenRestDependencyTest
: public ::testing::Test
{
public:
  void ConstructorTest()
  {
    OpenRestDependency open_rest_dependency;
    ASSERT_TRUE(open_rest_dependency.dependency_tree_.empty());
  }

  void OneMoveAddGetTest()
  {
    // rest = (AA)
    OpenRestDependency open_rest_dependency;
    
    const OpenRestListKey key = kMoveAA;
    open_rest_dependency.Add(key);

    const auto &tree = open_rest_dependency.dependency_tree_;

    ASSERT_TRUE(tree.empty());
  }

  void TwoMoveAddGetTest()
  {
    // rest = (AA, AB)
    vector<MovePosition> rest_move_list{kMoveAA, kMoveAB};
    OpenRestList open_rest_list(rest_move_list);
    const OpenRestListKey key = open_rest_list.GetOpenRestKey();

    OpenRestDependency open_rest_dependency;
    open_rest_dependency.Add(key);

    const auto &tree = open_rest_dependency.dependency_tree_;

    ASSERT_EQ(2, tree.size());

    {
      const auto &child_set = open_rest_dependency.GetChildSet(kMoveAA);
    
      ASSERT_EQ(1, child_set.size());
      ASSERT_TRUE(child_set.find(key) != child_set.end());

      set<OpenRestListKey> dependent_keys;
      open_rest_dependency.GetAllDependentKeys(kMoveAA, &dependent_keys);

      ASSERT_EQ(1, dependent_keys.size());
      ASSERT_TRUE(dependent_keys.find(key) != dependent_keys.end());
    }
    {
      const auto &child_set = open_rest_dependency.GetChildSet(kMoveAB);
      
      ASSERT_EQ(1, child_set.size());
      ASSERT_TRUE(child_set.find(key) != child_set.end());

      set<OpenRestListKey> dependent_keys;
      open_rest_dependency.GetAllDependentKeys(kMoveAB, &dependent_keys);

      ASSERT_EQ(1, dependent_keys.size());
      ASSERT_TRUE(dependent_keys.find(key) != dependent_keys.end());
    }
  }

  void ThreeMoveAddGetTest()
  {
    // rest = (AA, AB, AC)
    vector<MovePosition> rest_move_list{kMoveAA, kMoveAB, kMoveAC};
    OpenRestList open_rest_list(rest_move_list);
    const OpenRestListKey key = open_rest_list.GetOpenRestKey();

    OpenRestDependency open_rest_dependency;
    open_rest_dependency.Add(key);

    const auto &tree = open_rest_dependency.dependency_tree_;

    ASSERT_EQ(6, tree.size());

    static constexpr OpenRestListKey kSubKeyAA_AB = (kMoveAA << 8) | kMoveAB;
    static constexpr OpenRestListKey kSubKeyAA_AC = (kMoveAA << 8) | kMoveAC;
    static constexpr OpenRestListKey kSubKeyAB_AC = (kMoveAB << 8) | kMoveAC;

    {
      const auto &child_set = open_rest_dependency.GetChildSet(kMoveAA);
      
      ASSERT_EQ(2, child_set.size());
      ASSERT_TRUE(child_set.find(kSubKeyAA_AB) != child_set.end());
      ASSERT_TRUE(child_set.find(kSubKeyAA_AC) != child_set.end());

      set<OpenRestListKey> dependent_keys;
      open_rest_dependency.GetAllDependentKeys(kMoveAA, &dependent_keys);

      ASSERT_EQ(3, dependent_keys.size());
      ASSERT_TRUE(dependent_keys.find(kSubKeyAA_AB) != dependent_keys.end());
      ASSERT_TRUE(dependent_keys.find(kSubKeyAA_AC) != dependent_keys.end());
      ASSERT_TRUE(dependent_keys.find(key) != dependent_keys.end());
    }
    {
      const auto &child_set = open_rest_dependency.GetChildSet(kMoveAB);
      
      ASSERT_EQ(2, child_set.size());
      ASSERT_TRUE(child_set.find(kSubKeyAA_AB) != child_set.end());
      ASSERT_TRUE(child_set.find(kSubKeyAB_AC) != child_set.end());

      set<OpenRestListKey> dependent_keys;
      open_rest_dependency.GetAllDependentKeys(kMoveAB, &dependent_keys);

      ASSERT_EQ(3, dependent_keys.size());
      ASSERT_TRUE(dependent_keys.find(kSubKeyAA_AB) != dependent_keys.end());
      ASSERT_TRUE(dependent_keys.find(kSubKeyAB_AC) != dependent_keys.end());
      ASSERT_TRUE(dependent_keys.find(key) != dependent_keys.end());
    }
    {
      const auto &child_set = open_rest_dependency.GetChildSet(kMoveAC);
      
      ASSERT_EQ(2, child_set.size());
      ASSERT_TRUE(child_set.find(kSubKeyAA_AC) != child_set.end());
      ASSERT_TRUE(child_set.find(kSubKeyAB_AC) != child_set.end());

      set<OpenRestListKey> dependent_keys;
      open_rest_dependency.GetAllDependentKeys(kMoveAC, &dependent_keys);

      ASSERT_EQ(3, dependent_keys.size());
      ASSERT_TRUE(dependent_keys.find(kSubKeyAA_AC) != dependent_keys.end());
      ASSERT_TRUE(dependent_keys.find(kSubKeyAB_AC) != dependent_keys.end());
      ASSERT_TRUE(dependent_keys.find(key) != dependent_keys.end());
    }
    {
      const auto &child_set = open_rest_dependency.GetChildSet(kSubKeyAA_AB);
      
      ASSERT_EQ(1, child_set.size());
      ASSERT_TRUE(child_set.find(key) != child_set.end());

      set<OpenRestListKey> dependent_keys;
      open_rest_dependency.GetAllDependentKeys(kSubKeyAA_AB, &dependent_keys);

      ASSERT_EQ(1, dependent_keys.size());
      ASSERT_TRUE(dependent_keys.find(key) != dependent_keys.end());
    }
    {
      const auto &child_set = open_rest_dependency.GetChildSet(kSubKeyAA_AC);
      
      ASSERT_EQ(1, child_set.size());
      ASSERT_TRUE(child_set.find(key) != child_set.end());

      set<OpenRestListKey> dependent_keys;
      open_rest_dependency.GetAllDependentKeys(kSubKeyAA_AC, &dependent_keys);

      ASSERT_EQ(1, dependent_keys.size());
      ASSERT_TRUE(dependent_keys.find(key) != dependent_keys.end());
    }
    {
      const auto &child_set = open_rest_dependency.GetChildSet(kSubKeyAB_AC);
      
      ASSERT_EQ(1, child_set.size());
      ASSERT_TRUE(child_set.find(key) != child_set.end());

      set<OpenRestListKey> dependent_keys;
      open_rest_dependency.GetAllDependentKeys(kSubKeyAA_AC, &dependent_keys);

      ASSERT_EQ(1, dependent_keys.size());
      ASSERT_TRUE(dependent_keys.find(key) != dependent_keys.end());
    }
  }
};    // class OpenRestListTest

TEST_F(OpenRestDependencyTest, ConstructorTest)
{
  ConstructorTest();
}

TEST_F(OpenRestDependencyTest, OneMoveAddGetTest)
{
  OneMoveAddGetTest();
}

TEST_F(OpenRestDependencyTest, TwoMoveAddGetTest)
{
  TwoMoveAddGetTest();
}

TEST_F(OpenRestDependencyTest, ThreeMoveAddGetTest)
{
  ThreeMoveAddGetTest();
}


}   // namespace realcore