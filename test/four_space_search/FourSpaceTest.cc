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
    ASSERT_FALSE(four_space.GetOpponentFourInfo().IsOpponentFour());
  }

  void ConstructorMoveInitTest(){
    FourSpace four_space(kMoveHH, kMoveHI);
    
    ASSERT_EQ(1, four_space.gain_bit_.count());
    ASSERT_TRUE(four_space.gain_bit_[kMoveHH]);
    
    ASSERT_EQ(1, four_space.cost_bit_.count());
    ASSERT_TRUE(four_space.cost_bit_[kMoveHI]);

    ASSERT_FALSE(four_space.GetOpponentFourInfo().IsOpponentFour());
  }

  void ConstructorLocalInitTest(){
    FourSpace four_space_base(kMoveHH, kMoveHI);
    MoveBitSet mask_bit;

    mask_bit.set(kMoveHH);
    FourSpace four_space(four_space_base, mask_bit);
    
    ASSERT_EQ(1, four_space.gain_bit_.count());
    ASSERT_TRUE(four_space.gain_bit_[kMoveHH]);
    
    ASSERT_TRUE(four_space.cost_bit_.none());
    ASSERT_FALSE(four_space.GetOpponentFourInfo().IsOpponentFour());
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

  void SetOpponentFourTest(){
    {
      FourSpace four_space;
      MovePair four_move(kMoveHH, kMoveHI);
      four_space.SetOpponentFour(four_move);

      OpponentFourInfo expected;
      expected.SetOpponentFour(four_move);

      ASSERT_TRUE(four_space.GetOpponentFourInfo() == expected);
    }
    {
      FourSpace four_space;
      MovePair four_move(kMoveHH, kMoveHI);
      four_space.SetOpponentFour(four_move, kMoveHJ, kMoveHK);

      OpponentFourInfo expected;
      expected.SetOpponentFour(four_move, kMoveHJ, kMoveHK);

      ASSERT_TRUE(four_space.GetOpponentFourInfo() == expected);
    }
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

TEST_F(FourSpaceTest, SetOpponentFourTest){
  SetOpponentFourTest();
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

TEST_F(FourSpaceTest, IsSameGainCostBitTest)
{
  FourSpace space_1(kMoveAA, kMoveAB), space_2(kMoveAA, kMoveAB);
  MovePair opponent_four(kMoveAC, kMoveAD);

  space_1.SetOpponentFour(opponent_four);

  ASSERT_TRUE(space_1.IsSameGainCostBit(space_2));
  ASSERT_FALSE(space_1 == space_2);
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


