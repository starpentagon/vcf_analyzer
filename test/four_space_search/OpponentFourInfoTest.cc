#include "gtest/gtest.h"

#include "OpponentFourInfo.h"

using namespace std;

namespace realcore
{

class OpponentFourInfoTest
: public ::testing::Test
{
};   // class OpponentFourInfoTest

TEST_F(OpponentFourInfoTest, ConstructorTest)
{
  OpponentFourInfo opponent_four_info;

  ASSERT_FALSE(opponent_four_info.IsOpponentFour());
}

TEST_F(OpponentFourInfoTest, GetSetOpponentFourTest)
{
  {
    OpponentFourInfo opponent_four_info;
    MovePair four_move(kMoveAA, kMoveAB);
    opponent_four_info.SetOpponentFour(four_move);

    ASSERT_TRUE(opponent_four_info.IsOpponentFour());
    ASSERT_EQ(kMoveAA, opponent_four_info.GetFourPosition());
    ASSERT_EQ(kMoveAB, opponent_four_info.GetGuardPosition());
    ASSERT_TRUE(opponent_four_info.GetDisablingMoveList().empty());
  }
  {
    OpponentFourInfo opponent_four_info;
    MovePair four_move(kMoveAA, kMoveAB);
    opponent_four_info.SetOpponentFour(four_move, kMoveAC, kNullMove);

    ASSERT_TRUE(opponent_four_info.IsOpponentFour());
    ASSERT_EQ(kMoveAA, opponent_four_info.GetFourPosition());
    ASSERT_EQ(kMoveAB, opponent_four_info.GetGuardPosition());

    const auto& disabling_move_list = opponent_four_info.GetDisablingMoveList();
    ASSERT_EQ(1, disabling_move_list.size());
    ASSERT_EQ(kMoveAC, disabling_move_list[0]);
  }
  {
    OpponentFourInfo opponent_four_info;
    MovePair four_move(kMoveAA, kMoveAB);
    opponent_four_info.SetOpponentFour(four_move, kNullMove, kMoveAD);

    ASSERT_TRUE(opponent_four_info.IsOpponentFour());
    ASSERT_EQ(kMoveAA, opponent_four_info.GetFourPosition());
    ASSERT_EQ(kMoveAB, opponent_four_info.GetGuardPosition());

    const auto& disabling_move_list = opponent_four_info.GetDisablingMoveList();
    ASSERT_EQ(1, disabling_move_list.size());
    ASSERT_EQ(kMoveAD, disabling_move_list[0]);
  }
  {
    OpponentFourInfo opponent_four_info;
    MovePair four_move(kMoveAA, kMoveAB);
    opponent_four_info.SetOpponentFour(four_move, kMoveAE, kMoveAF);

    ASSERT_TRUE(opponent_four_info.IsOpponentFour());
    ASSERT_EQ(kMoveAA, opponent_four_info.GetFourPosition());
    ASSERT_EQ(kMoveAB, opponent_four_info.GetGuardPosition());

    const auto& disabling_move_list = opponent_four_info.GetDisablingMoveList();
    ASSERT_EQ(2, disabling_move_list.size());
    ASSERT_EQ(kMoveAE, disabling_move_list[0]);
    ASSERT_EQ(kMoveAF, disabling_move_list[1]);
  }
}

TEST_F(OpponentFourInfoTest, CompareOperTest)
{
  {
    OpponentFourInfo opponent_four_info_1, opponent_four_info_2;

    ASSERT_TRUE(opponent_four_info_1 == opponent_four_info_2);    
  }
  {
    OpponentFourInfo opponent_four_info_1, opponent_four_info_2;
    MovePair four_move(kMoveAA, kMoveAB);

    opponent_four_info_1.SetOpponentFour(four_move);
    ASSERT_FALSE(opponent_four_info_1 == opponent_four_info_2);    

    opponent_four_info_2.SetOpponentFour(four_move);
    ASSERT_TRUE(opponent_four_info_1 == opponent_four_info_2);    
  }
  {
    OpponentFourInfo opponent_four_info_1, opponent_four_info_2;
    MovePair four_move(kMoveAA, kMoveAB);

    opponent_four_info_1.SetOpponentFour(four_move, kMoveAE, kMoveAF);
    ASSERT_FALSE(opponent_four_info_1 == opponent_four_info_2);    

    opponent_four_info_2.SetOpponentFour(four_move, kMoveAE, kMoveAF);
    ASSERT_TRUE(opponent_four_info_1 == opponent_four_info_2);    
  }
}

TEST_F(OpponentFourInfoTest, AssignOperTest)
{
  {
    OpponentFourInfo opponent_four_info_1, opponent_four_info_2;
    MovePair four_move(kMoveAA, kMoveAB);
  
    opponent_four_info_1.SetOpponentFour(four_move);
    ASSERT_FALSE(opponent_four_info_1 == opponent_four_info_2);
  
    opponent_four_info_2 = opponent_four_info_1;
    ASSERT_TRUE(opponent_four_info_1 == opponent_four_info_2);    
  }
  {
    OpponentFourInfo opponent_four_info_1, opponent_four_info_2;
    MovePair four_move(kMoveAA, kMoveAB);
  
    opponent_four_info_1.SetOpponentFour(four_move);
    ASSERT_FALSE(opponent_four_info_1 == opponent_four_info_2);
  
    opponent_four_info_1 = opponent_four_info_2;
    ASSERT_TRUE(opponent_four_info_1 == opponent_four_info_2);    
  }
}

}   // namespace realcore
