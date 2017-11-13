#include "gtest/gtest.h"

#include "FourSpace.h"

using namespace std;

namespace realcore
{
class FourSpaceTest
: public ::testing::Test
{
public:
  void ConstructorTest(){
    FourSpace four_space;

    ASSERT_TRUE(four_space.gain_bit_.none());
    ASSERT_TRUE(four_space.cost_bit_.none());
  }

  void AddMoveTest()
  {
    FourSpace four_space;

    four_space.Add(kMoveHH, kMoveHI);
    
    ASSERT_TRUE(four_space.gain_bit_[kMoveHH]);
    ASSERT_EQ(1, four_space.gain_bit_.count());
    ASSERT_TRUE(four_space.cost_bit_[kMoveHI]);
    ASSERT_EQ(1, four_space.cost_bit_.count());
  }

  void AddSeqTest()
  {
    FourSpace four_space;
    four_space.Add(kMoveHH, kMoveHI);
    four_space.Add(kMoveHJ, kMoveHK);
    
    FourSpace four_space_added;
    four_space_added.Add(kMoveHL, kMoveHM);
    four_space_added.Add(four_space);
    
    ASSERT_TRUE(four_space_added.gain_bit_[kMoveHH]);
    ASSERT_TRUE(four_space_added.gain_bit_[kMoveHJ]);
    ASSERT_TRUE(four_space_added.gain_bit_[kMoveHL]);
    ASSERT_EQ(3, four_space_added.gain_bit_.count());
    
    ASSERT_TRUE(four_space_added.cost_bit_[kMoveHI]);
    ASSERT_TRUE(four_space_added.cost_bit_[kMoveHK]);
    ASSERT_TRUE(four_space_added.cost_bit_[kMoveHM]);
    ASSERT_EQ(3, four_space_added.cost_bit_.count());
  }

  void ConflictMoveTest()
  {
    FourSpace four_space;
    four_space.Add(kMoveHH, kMoveHI);

    // 競合しない: 別の位置
    ASSERT_FALSE(four_space.IsConflict(kMoveAA, kMoveAB));

    // 競合
    ASSERT_TRUE(four_space.IsConflict(kMoveHH, kMoveAA));
    ASSERT_TRUE(four_space.IsConflict(kMoveHI, kMoveAA));
    ASSERT_TRUE(four_space.IsConflict(kMoveAA, kMoveHH));
    ASSERT_TRUE(four_space.IsConflict(kMoveAA, kMoveHI));
  }

  void PuttableTest(){
    FourSpace four_space;
    four_space.Add(kMoveHH, kMoveHI);
    four_space.Add(kMoveHJ, kMoveHK);
    
    // 設置可能: 別の位置
    {
      FourSpace test_space;
      test_space.Add(kMoveAA, kMoveAB);
      test_space.Add(kMoveAC, kMoveAD);
      ASSERT_TRUE(four_space.IsPuttable(test_space));
    }

    // 実現可能: 四ノビを一部共有
    {
      FourSpace test_space;
      test_space.Add(kMoveAA, kMoveAB);
      test_space.Add(kMoveHH, kMoveHI);
      ASSERT_TRUE(four_space.IsPuttable(test_space));
    }

    // 実現可能: 四ノビをすべて共有
    {
      FourSpace test_space;
      test_space.Add(kMoveHH, kMoveHI);
      test_space.Add(kMoveHJ, kMoveHK);
      ASSERT_TRUE(four_space.IsPuttable(test_space));
    }

    // 実現可能: 四ノビを包含
    {
      FourSpace test_space;
      test_space.Add(kMoveHH, kMoveHI);
      test_space.Add(kMoveHJ, kMoveHK);
      test_space.Add(kMoveAA, kMoveAB);
      ASSERT_TRUE(four_space.IsPuttable(test_space));
    }
  }
};

TEST_F(FourSpaceTest, ConstructorTest)
{
  ConstructorTest();
}

TEST_F(FourSpaceTest, AddMoveTest)
{
  AddMoveTest();
}

TEST_F(FourSpaceTest, AddSeqTest)
{
  AddSeqTest();
}

TEST_F(FourSpaceTest, ConflictMoveTest)
{
  ConflictMoveTest();
}

TEST_F(FourSpaceTest, PuttableTest)
{
  PuttableTest();
}

TEST_F(FourSpaceTest, CompOperTest)
{
  FourSpace space_1, space_2;

  ASSERT_TRUE(space_1 == space_2);
  ASSERT_FALSE(space_1 != space_2);

  space_1.Add(kMoveAA, kMoveAB);
  ASSERT_FALSE(space_1 == space_2);
  ASSERT_TRUE(space_1 != space_2);

  space_2.Add(kMoveAA, kMoveAB);
  ASSERT_TRUE(space_1 == space_2);
  ASSERT_FALSE(space_1 != space_2);

  FourSpace space_3;
  space_3.Add(kMoveAB, kMoveAA);
  ASSERT_FALSE(space_1 == space_3);
  ASSERT_TRUE(space_1 != space_3);
}

TEST_F(FourSpaceTest, AssignOperTest)
{
  FourSpace space_1, space_2;

  space_1.Add(kMoveAA, kMoveAB);
  ASSERT_FALSE(space_1 == space_2);
  ASSERT_TRUE(space_1 != space_2);

  space_2 = space_1;
  ASSERT_TRUE(space_1 == space_2);
  ASSERT_FALSE(space_1 != space_2);
}

TEST_F(FourSpaceTest, GetNeighborhoodGainBitTest)
{
  FourSpace space;

  space.Add(kMoveHH, kMoveHI);
  space.Add(kMoveAA, kMoveCA);

  MoveBitSet neighbor_center, neighbor_corner;

  GetLineNeighborhoodBit(kMoveHH, 5, &neighbor_center);
  GetLineNeighborhoodBit(kMoveBA, 1, &neighbor_corner);
  
  {
    const auto &bit = space.GetNeighborhoodGainBit(neighbor_center);

    ASSERT_EQ(1, bit.count());
    ASSERT_TRUE(bit[kMoveHH]);
  }
  {
    const auto &bit = space.GetNeighborhoodGainBit(neighbor_corner);

    ASSERT_EQ(1, bit.count());
    ASSERT_TRUE(bit[kMoveAA]);
  }
}

TEST_F(FourSpaceTest, GetNeighborhoodCostBitTest)
{
  FourSpace space;

  space.Add(kMoveHH, kMoveHI);
  space.Add(kMoveAA, kMoveCA);

  MoveBitSet neighbor_center, neighbor_corner;

  GetLineNeighborhoodBit(kMoveHH, 5, &neighbor_center);
  GetLineNeighborhoodBit(kMoveBA, 1, &neighbor_corner);
  
  {
    const auto &bit = space.GetNeighborhoodCostBit(neighbor_center);

    ASSERT_EQ(1, bit.count());
    ASSERT_TRUE(bit[kMoveHI]);
  }
  {
    const auto &bit = space.GetNeighborhoodCostBit(neighbor_corner);

    ASSERT_EQ(1, bit.count());
    ASSERT_TRUE(bit[kMoveCA]);
  }
}

}


