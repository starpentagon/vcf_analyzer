#include "gtest/gtest.h"

#include "FourSpace.h"
#include "RelaxedFour.h"

using namespace std;

namespace realcore
{
class RelaxedFourTest
: public ::testing::Test
{
public:
};

TEST_F(RelaxedFourTest, ConstructorTest)
{
  vector<MovePosition> rest_list{{kMoveAD, kMoveAC}};
  RelaxedFour relaxed_four(kMoveAA, kMoveAB, rest_list);

  ASSERT_EQ(kMoveAA, relaxed_four.GetGainPosition());
  ASSERT_EQ(kMoveAB, relaxed_four.GetCostPosition());
  ASSERT_EQ(rest_list, relaxed_four.GetRestPositionList());
}

TEST_F(RelaxedFourTest, CompareOperTest)
{
  {
    // 獲得路が異なるケース
    vector<MovePosition> rest_list_1{{kMoveAD, kMoveAC}};
    RelaxedFour relaxed_four_1(kMoveAA, kMoveAB, rest_list_1);

    vector<MovePosition> rest_list_2{{kMoveAD, kMoveAC}};
    RelaxedFour relaxed_four_2(kMoveOO, kMoveAB, rest_list_2);

    ASSERT_FALSE(relaxed_four_1 == relaxed_four_2);
    ASSERT_TRUE(relaxed_four_1 != relaxed_four_2);
  }
  {
    // 損失路が異なるケース
    vector<MovePosition> rest_list_1{{kMoveAD, kMoveAC}};
    RelaxedFour relaxed_four_1(kMoveAA, kMoveAB, rest_list_1);

    vector<MovePosition> rest_list_2{{kMoveAD, kMoveAC}};
    RelaxedFour relaxed_four_2(kMoveAA, kMoveOO, rest_list_2);

    ASSERT_FALSE(relaxed_four_1 == relaxed_four_2);
    ASSERT_TRUE(relaxed_four_1 != relaxed_four_2);
  }
  {
    // 残路が異なるケース
    vector<MovePosition> rest_list_1{{kMoveAD, kMoveAC}};
    RelaxedFour relaxed_four_1(kMoveAA, kMoveAB, rest_list_1);

    vector<MovePosition> rest_list_2{{kMoveAE, kMoveAC}};
    RelaxedFour relaxed_four_2(kMoveAA, kMoveAB, rest_list_2);

    ASSERT_FALSE(relaxed_four_1 == relaxed_four_2);
    ASSERT_TRUE(relaxed_four_1 != relaxed_four_2);
  }
}

TEST_F(RelaxedFourTest, AssignOperTest)
{
  vector<MovePosition> rest_list_1{{kMoveAD, kMoveAC}};

  RelaxedFour relaxed_four_1(kMoveAA, kMoveAB, rest_list_1);

  vector<MovePosition> rest_list_2{{kMoveAE, kMoveAC}};
  RelaxedFour relaxed_four_2(kMoveAA, kMoveAB, rest_list_2);

  ASSERT_FALSE(relaxed_four_1 == relaxed_four_2);

  relaxed_four_2 = relaxed_four_1;
  ASSERT_TRUE(relaxed_four_1 == relaxed_four_2);
}

TEST_F(RelaxedFourTest, GetKeyTest)
{
  {
    vector<MovePosition> rest_list{{kMoveAE, kMoveAD, kMoveAC}};
    RelaxedFour relaxed_four(kMoveAA, kMoveAB, rest_list);
    constexpr uint64_t key = static_cast<uint64_t>(kMoveAA) << 32 | kMoveAB << 24 | kMoveAE << 16 | kMoveAD << 8 | kMoveAC;

    ASSERT_EQ(key, relaxed_four.GetKey());
  }
  {
    vector<MovePosition> rest_list;
    RelaxedFour relaxed_four(kMoveAA, kMoveAB, rest_list);
    constexpr uint64_t key = static_cast<uint64_t>(kMoveAA) << 32 | kMoveAB << 24;

    ASSERT_EQ(key, relaxed_four.GetKey());
  }
}

}