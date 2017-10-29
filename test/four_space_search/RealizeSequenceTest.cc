#include "gtest/gtest.h"

#include "RealizeSequence.h"

using namespace std;

namespace realcore
{
class RealizeSequenceTest
: public ::testing::Test
{
public:
  void ConstructorTest(){
    RealizeSequence realize_sequence;

    ASSERT_TRUE(realize_sequence.gain_bit_.none());
    ASSERT_TRUE(realize_sequence.cost_bit_.none());
    ASSERT_TRUE(realize_sequence.gain_cost_map_.empty());
  }

  void AddMoveTest()
  {
    RealizeSequence realize_sequence;

    realize_sequence.Add(kMoveHH, kMoveHI);
    
    ASSERT_TRUE(realize_sequence.gain_bit_[kMoveHH]);
    ASSERT_EQ(1, realize_sequence.gain_bit_.count());
    ASSERT_TRUE(realize_sequence.cost_bit_[kMoveHI]);
    ASSERT_EQ(1, realize_sequence.cost_bit_.count());

    ASSERT_EQ(kMoveHI, realize_sequence.gain_cost_map_[kMoveHH]);
    ASSERT_EQ(1, realize_sequence.gain_cost_map_.size());
  }

  void AddSeqTest()
  {
    RealizeSequence realize_sequence;
    realize_sequence.Add(kMoveHH, kMoveHI);
    realize_sequence.Add(kMoveHJ, kMoveHK);
    
    RealizeSequence realize_sequence_added;
    realize_sequence_added.Add(kMoveHL, kMoveHM);
    realize_sequence_added.Add(realize_sequence);
    
    ASSERT_TRUE(realize_sequence_added.gain_bit_[kMoveHH]);
    ASSERT_TRUE(realize_sequence_added.gain_bit_[kMoveHJ]);
    ASSERT_TRUE(realize_sequence_added.gain_bit_[kMoveHL]);
    ASSERT_EQ(3, realize_sequence_added.gain_bit_.count());
    
    ASSERT_TRUE(realize_sequence_added.cost_bit_[kMoveHI]);
    ASSERT_TRUE(realize_sequence_added.cost_bit_[kMoveHK]);
    ASSERT_TRUE(realize_sequence_added.cost_bit_[kMoveHM]);
    ASSERT_EQ(3, realize_sequence_added.cost_bit_.count());

    ASSERT_EQ(kMoveHI, realize_sequence_added.gain_cost_map_[kMoveHH]);
    ASSERT_EQ(kMoveHK, realize_sequence_added.gain_cost_map_[kMoveHJ]);
    ASSERT_EQ(kMoveHM, realize_sequence_added.gain_cost_map_[kMoveHL]);
    ASSERT_EQ(3, realize_sequence_added.gain_cost_map_.size());
  }

  void ConflictMoveTest()
  {
    RealizeSequence realize_sequence;
    realize_sequence.Add(kMoveHH, kMoveHI);

    // 競合しない: 別の位置
    ASSERT_FALSE(realize_sequence.IsConflict(kMoveAA, kMoveAB));

    // 競合しない: 四ノビを共有
    ASSERT_FALSE(realize_sequence.IsConflict(kMoveHH, kMoveHI));

    // 競合: 獲得路に対する損失路が異なる
    ASSERT_TRUE(realize_sequence.IsConflict(kMoveHH, kMoveHJ));

    // 競合: 獲得路が異なるが損失路が一致
    ASSERT_TRUE(realize_sequence.IsConflict(kMoveAA, kMoveHH));
    ASSERT_TRUE(realize_sequence.IsConflict(kMoveAA, kMoveHI));
  }

  void ConflictSeqTest(){
    RealizeSequence realize_sequence;
    realize_sequence.Add(kMoveHH, kMoveHI);
    realize_sequence.Add(kMoveHJ, kMoveHK);
    
    // 競合しない: 別の位置
    {
      RealizeSequence test_sequence;
      test_sequence.Add(kMoveAA, kMoveAB);
      test_sequence.Add(kMoveAC, kMoveAD);
      ASSERT_FALSE(realize_sequence.IsConflict(test_sequence));
    }

    // 競合しない: 四ノビを一部共有
    {
      RealizeSequence test_sequence;
      test_sequence.Add(kMoveAA, kMoveAB);
      test_sequence.Add(kMoveHH, kMoveHI);
      ASSERT_FALSE(realize_sequence.IsConflict(test_sequence));
    }

    // 競合しない: 四ノビをすべて共有
    {
      RealizeSequence test_sequence;
      test_sequence.Add(kMoveHH, kMoveHI);
      test_sequence.Add(kMoveHJ, kMoveHK);
      ASSERT_FALSE(realize_sequence.IsConflict(test_sequence));
    }

    // 競合しない: 四ノビを包含
    {
      RealizeSequence test_sequence;
      test_sequence.Add(kMoveHH, kMoveHI);
      test_sequence.Add(kMoveHJ, kMoveHK);
      test_sequence.Add(kMoveAA, kMoveAB);
      ASSERT_FALSE(realize_sequence.IsConflict(test_sequence));
    }

    // 競合: 獲得路に対する損失路が異なる
    {
      RealizeSequence test_sequence;
      test_sequence.Add(kMoveHH, kMoveHK);
      test_sequence.Add(kMoveAA, kMoveAB);
      ASSERT_TRUE(realize_sequence.IsConflict(test_sequence));
    }

    // 競合: 獲得路が異なるが損失路が一致
    {
      RealizeSequence test_sequence;
      test_sequence.Add(kMoveHG, kMoveHH);
      ASSERT_TRUE(realize_sequence.IsConflict(test_sequence));
    }
    {
      RealizeSequence test_sequence;
      test_sequence.Add(kMoveHG, kMoveHI);
      ASSERT_TRUE(realize_sequence.IsConflict(test_sequence));
    }
  }
};

TEST_F(RealizeSequenceTest, ConstructorTest)
{
  ConstructorTest();
}

TEST_F(RealizeSequenceTest, AddMoveTest)
{
  AddMoveTest();
}

TEST_F(RealizeSequenceTest, AddSeqTest)
{
  AddSeqTest();
}

TEST_F(RealizeSequenceTest, ConflictMoveTest)
{
  ConflictMoveTest();
}

TEST_F(RealizeSequenceTest, ConflictSeqTest)
{
  ConflictSeqTest();
}

TEST_F(RealizeSequenceTest, CompOperTest)
{
  RealizeSequence seq_1, seq_2;

  ASSERT_TRUE(seq_1 == seq_2);
  ASSERT_FALSE(seq_1 != seq_2);

  seq_1.Add(kMoveAA, kMoveAB);
  ASSERT_FALSE(seq_1 == seq_2);
  ASSERT_TRUE(seq_1 != seq_2);

  seq_2.Add(kMoveAA, kMoveAB);
  ASSERT_TRUE(seq_1 == seq_2);
  ASSERT_FALSE(seq_1 != seq_2);

  RealizeSequence seq_3;
  seq_3.Add(kMoveAB, kMoveAA);
  ASSERT_FALSE(seq_1 == seq_3);
  ASSERT_TRUE(seq_1 != seq_3);
}

TEST_F(RealizeSequenceTest, AssignOperTest)
{
  RealizeSequence seq_1, seq_2;

  seq_1.Add(kMoveAA, kMoveAB);
  ASSERT_FALSE(seq_1 == seq_2);
  ASSERT_TRUE(seq_1 != seq_2);

  seq_2 = seq_1;
  ASSERT_TRUE(seq_1 == seq_2);
  ASSERT_FALSE(seq_1 != seq_2);
}
}


