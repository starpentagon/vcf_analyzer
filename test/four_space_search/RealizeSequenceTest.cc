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

  void AddTest1()
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

  void AddTest2()
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
};

TEST_F(RealizeSequenceTest, ConstructorTest)
{
  ConstructorTest();
}

TEST_F(RealizeSequenceTest, AddTest1)
{
  AddTest1();
}

TEST_F(RealizeSequenceTest, AddTest2)
{
  AddTest2();
}
}


