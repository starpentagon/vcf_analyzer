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

  void ConstructorMoveInitTest(){
    FourSpace four_space(kMoveHH, kMoveHI);
    
    ASSERT_EQ(1, four_space.gain_bit_.count());
    ASSERT_TRUE(four_space.gain_bit_[kMoveHH]);
    
    ASSERT_EQ(1, four_space.cost_bit_.count());
    ASSERT_TRUE(four_space.cost_bit_[kMoveHI]);
  }

  void ConstructorLocalInitTest(){
    FourSpace four_space_base(kMoveHH, kMoveHI);
    MoveBitSet mask_bit;

    mask_bit.set(kMoveHH);
    FourSpace four_space(four_space_base, mask_bit);
    
    ASSERT_EQ(1, four_space.gain_bit_.count());
    ASSERT_TRUE(four_space.gain_bit_[kMoveHH]);
    
    ASSERT_TRUE(four_space.cost_bit_.none());
  }

  void AddMoveTest()
  {
    FourSpace four_space;

    four_space.Add(kMoveHH, kMoveHI);
    
    ASSERT_EQ(1, four_space.gain_bit_.count());
    ASSERT_TRUE(four_space.gain_bit_[kMoveHH]);
    ASSERT_EQ(1, four_space.cost_bit_.count());
    ASSERT_TRUE(four_space.cost_bit_[kMoveHI]);

    four_space.Add(kMoveAA, kMoveAB);

    ASSERT_EQ(2, four_space.gain_bit_.count());
    ASSERT_TRUE(four_space.gain_bit_[kMoveHH]);
    ASSERT_TRUE(four_space.gain_bit_[kMoveAA]);
    ASSERT_EQ(2, four_space.cost_bit_.count());
    ASSERT_TRUE(four_space.cost_bit_[kMoveHI]);
    ASSERT_TRUE(four_space.cost_bit_[kMoveAB]);
  }

  void AddSpaceTest()
  {
    FourSpace four_space;
    four_space.Add(kMoveHH, kMoveHI);
    four_space.Add(kMoveHJ, kMoveHK);
    
    FourSpace four_space_added;
    four_space_added.Add(kMoveHH, kMoveHI);
    four_space_added.Add(kMoveHL, kMoveHM);
    four_space_added.Add(four_space);
    
    ASSERT_EQ(3, four_space_added.gain_bit_.count());
    ASSERT_TRUE(four_space_added.gain_bit_[kMoveHH]);
    ASSERT_TRUE(four_space_added.gain_bit_[kMoveHJ]);
    ASSERT_TRUE(four_space_added.gain_bit_[kMoveHL]);
    
    ASSERT_EQ(3, four_space_added.cost_bit_.count());
    ASSERT_TRUE(four_space_added.cost_bit_[kMoveHI]);
    ASSERT_TRUE(four_space_added.cost_bit_[kMoveHK]);
    ASSERT_TRUE(four_space_added.cost_bit_[kMoveHM]);
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

    // 設置可能: 四ノビを一部共有
    {
      FourSpace test_space;
      test_space.Add(kMoveAA, kMoveAB);
      test_space.Add(kMoveHH, kMoveHI);
      ASSERT_TRUE(four_space.IsPuttable(test_space));
    }

    // 設置可能: 四ノビをすべて共有
    {
      FourSpace test_space;
      test_space.Add(kMoveHH, kMoveHI);
      test_space.Add(kMoveHJ, kMoveHK);
      ASSERT_TRUE(four_space.IsPuttable(test_space));
    }

    // 設置可能: 四ノビを包含
    {
      FourSpace test_space;
      test_space.Add(kMoveHH, kMoveHI);
      test_space.Add(kMoveHJ, kMoveHK);
      test_space.Add(kMoveAA, kMoveAB);
      ASSERT_TRUE(four_space.IsPuttable(test_space));
    }

    // 均等ではないが設置可能
    {
      FourSpace test_space;
      test_space.Add(kMoveHH, kMoveAA);
      ASSERT_TRUE(four_space.IsPuttable(test_space));
    }
  }

  void IsBalancedTest()
  {
    FourSpace four_space;
    four_space.Add(kMoveHH, kMoveHI);
    four_space.Add(kMoveHJ, kMoveHK);
    
    // 均等
    {
      FourSpace test_space(kMoveHJ, kMoveHK);
      test_space.Add(kMoveAA, kMoveAB);
      test_space.Add(four_space);
      ASSERT_TRUE(test_space.IsBalanced());
    }

    // 均等ではない
    {
      FourSpace test_space(kMoveHH, kMoveAA);
      test_space.Add(four_space);
      ASSERT_FALSE(test_space.IsBalanced());
    }
  }
};

TEST_F(FourSpaceTest, ConstructorTest)
{
  ConstructorTest();
  ConstructorMoveInitTest();
  ConstructorLocalInitTest();
}

TEST_F(FourSpaceTest, AddMoveTest)
{
  AddMoveTest();
}

TEST_F(FourSpaceTest, AddSpaceTest)
{
  AddSpaceTest();
}

TEST_F(FourSpaceTest, ConflictMoveTest)
{
  ConflictMoveTest();
}

TEST_F(FourSpaceTest, PuttableTest)
{
  PuttableTest();
}

TEST_F(FourSpaceTest, IsBalancedTest)
{
  IsBalancedTest();
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

  const MoveBitSet &neighbor_center = GetLineNeighborhoodBit<5>(kMoveHH);
  const MoveBitSet &neighbor_corner = GetLineNeighborhoodBit<1>(kMoveAA);
  
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

  const MoveBitSet &neighbor_center = GetLineNeighborhoodBit<5>(kMoveHH);
  const MoveBitSet &neighbor_corner = GetLineNeighborhoodBit<1>(kMoveBA);

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

TEST_F(FourSpaceTest, CalcHashValueTest)
{
  {
    FourSpace four_space_1(kMoveAA, kMoveAB);
    FourSpace four_space_2(kMoveAA, kMoveAB);

    const auto hash_1 = four_space_1.CalcHashValue();
    const auto hash_2 = four_space_2.CalcHashValue();

    ASSERT_EQ(hash_1, hash_2);
  }
  {
    FourSpace four_space_1(kMoveAA, kMoveAB);
    FourSpace four_space_2(kMoveAA, kMoveAC);

    const auto hash_1 = four_space_1.CalcHashValue();
    const auto hash_2 = four_space_2.CalcHashValue();

    ASSERT_NE(hash_1, hash_2);
  }
  {
    FourSpace four_space_1(kMoveAA, kMoveAB);
    FourSpace four_space_2(kMoveAC, kMoveAD);

    const auto hash_1 = four_space_1.CalcHashValue();
    const auto hash_2 = four_space_2.CalcHashValue();

    ASSERT_NE(hash_1, hash_2);
  }
  {
    FourSpace four_space_1(kMoveAA, kMoveAB);
    FourSpace four_space_2(kMoveAB, kMoveAA);

    const auto hash_1 = four_space_1.CalcHashValue();
    const auto hash_2 = four_space_2.CalcHashValue();

    ASSERT_NE(hash_1, hash_2);
  }
}
}


