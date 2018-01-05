#include "gtest/gtest.h"
#include "OpenRestList.h"

using namespace std;

namespace realcore
{
  
class OpenRestListTest
: public ::testing::Test
{
public:
  void IsConsistentTest()
  {
    OpenRestList open_rest_list;
    ASSERT_TRUE(open_rest_list.IsConsistent());

    open_rest_list.Add(kMoveAA);
    ASSERT_TRUE(open_rest_list.IsConsistent());

    open_rest_list.Add(kMoveAB);
    ASSERT_TRUE(open_rest_list.IsConsistent());
    
    open_rest_list.open_rest_list_[0] = kMoveAC;
    ASSERT_FALSE(open_rest_list.IsConsistent());
  }
};  // class OpenRestListTest

TEST_F(OpenRestListTest, ConstructorTest)
{
  OpenRestList open_rest_list;
  ASSERT_TRUE(open_rest_list.empty());
}

TEST_F(OpenRestListTest, MoveListConstructorTest)
{
  vector<MovePosition> move_list{kMoveAA, kMoveAC, kMoveAB};
  OpenRestList open_rest_list(move_list);

  vector<MovePosition> expected{kMoveAA, kMoveAB, kMoveAC};
  ASSERT_TRUE(open_rest_list.GetOpenRestMoveList() == expected);
}

TEST_F(OpenRestListTest, CopyConstructorTest)
{
  vector<MovePosition> move_list{kMoveAA, kMoveAC, kMoveAB};
  OpenRestList open_rest_list_1(move_list);
  OpenRestList open_rest_list_2(open_rest_list_1);

  ASSERT_TRUE(open_rest_list_1.GetOpenRestMoveList() == open_rest_list_2.GetOpenRestMoveList());
}

TEST_F(OpenRestListTest, AddTest)
{
  OpenRestList open_rest_list;
  open_rest_list.Add(kMoveAB);

  {
    const auto& rest_move_list = open_rest_list.GetOpenRestMoveList();

    ASSERT_EQ(1, rest_move_list.size());
    ASSERT_EQ(kMoveAB, rest_move_list[0]);
  }

  open_rest_list.Add(kMoveAA);

  {
    const auto& rest_move_list = open_rest_list.GetOpenRestMoveList();

    ASSERT_EQ(2, rest_move_list.size());
    ASSERT_EQ(kMoveAA, rest_move_list[0]);
    ASSERT_EQ(kMoveAB, rest_move_list[1]);
  }
}

TEST_F(OpenRestListTest, EmptyTest)
{
  OpenRestList open_rest_list;

  ASSERT_TRUE(open_rest_list.empty());

  open_rest_list.Add(kMoveAA);
  ASSERT_FALSE(open_rest_list.empty());
}

TEST_F(OpenRestListTest, GetOpenRestKeyTest)
{
  OpenRestList open_rest_list;

  ASSERT_EQ(0ULL, open_rest_list.GetOpenRestKey());

  open_rest_list.Add(kMoveAA);
  uint64_t expected = kMoveAA;

  ASSERT_EQ(expected, open_rest_list.GetOpenRestKey());
  
  open_rest_list.Add(kMoveAC);
  expected = (kMoveAA << 8) | kMoveAC;

  ASSERT_EQ(expected, open_rest_list.GetOpenRestKey());

  open_rest_list.Add(kMoveAB);
  expected = (kMoveAA << 16) | (kMoveAB << 8) | kMoveAC;

  ASSERT_EQ(expected, open_rest_list.GetOpenRestKey());
}

TEST_F(OpenRestListTest, AssignOperTest)
{
  vector<MovePosition> move_list{kMoveAA, kMoveAC, kMoveAB};
  OpenRestList open_rest_list_1(move_list);
  OpenRestList open_rest_list_2;

  ASSERT_FALSE(open_rest_list_1 == open_rest_list_2);

  open_rest_list_2 = open_rest_list_1;
  ASSERT_TRUE(open_rest_list_1 == open_rest_list_2);
}

TEST_F(OpenRestListTest, CompareOperTest)
{
  {
    OpenRestList open_rest_list_1;
    OpenRestList open_rest_list_2;

    ASSERT_TRUE(open_rest_list_1 == open_rest_list_2);
  }
  {
    vector<MovePosition> move_list{kMoveAA, kMoveAC, kMoveAB};
    OpenRestList open_rest_list_1(move_list);
    OpenRestList open_rest_list_2;

    ASSERT_FALSE(open_rest_list_1 == open_rest_list_2);
  }
  {
    vector<MovePosition> move_list_1{kMoveAA, kMoveAC, kMoveAB};
    vector<MovePosition> move_list_2{kMoveAC, kMoveAB, kMoveAA};
    OpenRestList open_rest_list_1(move_list_1);
    OpenRestList open_rest_list_2(move_list_2);

    ASSERT_TRUE(open_rest_list_1 == open_rest_list_2);
  }
}

TEST_F(OpenRestListTest, IsConsistentTest)
{
  IsConsistentTest();
}

TEST_F(OpenRestListTest, GetOpenRestBitTest)
{
  vector<MovePosition> move_list{kMoveAA, kMoveAC, kMoveAB};
  OpenRestList open_rest_list(move_list);

  const auto rest_key = open_rest_list.GetOpenRestKey();
  MoveBitSet move_bit;
  GetOpenRestBit(rest_key, &move_bit);

  ASSERT_EQ(3, move_bit.count());
  ASSERT_TRUE(move_bit[kMoveAA]);
  ASSERT_TRUE(move_bit[kMoveAB]);
  ASSERT_TRUE(move_bit[kMoveAC]);
}

TEST_F(OpenRestListTest, GetParentOpenRestListKeyTest)
{
  vector<MovePosition> rest_move_list{kMoveAA, kMoveAB, kMoveAC};
  OpenRestList open_rest_list(rest_move_list);

  const auto key = open_rest_list.GetOpenRestKey();
  
  {
  const auto parent_key = GetParentOpenRestListKey(kMoveAA, key);
  const OpenRestListKey expect = (kMoveAB << 8) | kMoveAC;
  ASSERT_EQ(expect, parent_key);
  }
  {
  const auto parent_key = GetParentOpenRestListKey(kMoveAB, key);
  const OpenRestListKey expect = (kMoveAA << 8) | kMoveAC;
  ASSERT_EQ(expect, parent_key);
  }
  {
  const auto parent_key = GetParentOpenRestListKey(kMoveAC, key);
  const OpenRestListKey expect = (kMoveAA << 8) | kMoveAB;
  ASSERT_EQ(expect, parent_key);
  }
}

}   // namespace realcore

