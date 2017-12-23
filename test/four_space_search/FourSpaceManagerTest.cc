#include "gtest/gtest.h"
#include "FourSpaceManager.h"

using namespace std;

namespace realcore
{
class FourSpaceManagerTest
: public ::testing::Test
{
public:
  void ConstructorTest(){
    MoveList move_list;
    FourSpaceManager four_space_manager(move_list);

    ASSERT_EQ(1, four_space_manager.four_space_list_.size());   // kInvalidFourSpaceIDに対応する要素を追加するためサイズは1
    ASSERT_TRUE(four_space_manager.open_rest_key_four_space_id_.empty());
  }

  void GetFourSpaceIDTest()
  {
    MoveList move_list;
    FourSpaceManager four_space_manager(move_list);
    FourSpace empty_four_space;

    ASSERT_EQ(kInvalidFourSpaceID, four_space_manager.GetFourSpaceID(empty_four_space));

    FourSpace four_space_1(kMoveAA, kMoveAB);
    four_space_manager.RegisterFourSpace(four_space_1);

    ASSERT_EQ(1, four_space_manager.GetFourSpaceID(four_space_1));
    
    FourSpace four_space_2(kMoveAB, kMoveAA);
    ASSERT_EQ(kInvalidFourSpaceID, four_space_manager.GetFourSpaceID(four_space_2));
  }
};

TEST_F(FourSpaceManagerTest, ConstructorTest)
{
  ConstructorTest();
}

TEST_F(FourSpaceManagerTest, AddGetSigleElementTest)
{
  static constexpr MovePosition gain = kMoveAA;
  static constexpr MovePosition cost = kMoveAB;
  
  FourSpace four_space(gain, cost);
  
  MoveList move_list;
  FourSpaceManager four_space_manager(move_list);
  vector<RestKeyFourSpace> added_list;

  four_space_manager.AddFourSpace<kBlackTurn>(gain, cost, four_space, &added_list);

  ASSERT_EQ(1, added_list.size());

  const auto rest_key = added_list[0].first;
  const auto four_space_id = added_list[0].second;

  ASSERT_EQ(gain, rest_key);
  ASSERT_EQ(1, four_space_id);

  const auto &manager_four_space = four_space_manager.GetFourSpace(four_space_id);
  ASSERT_TRUE(manager_four_space == four_space);
}

TEST_F(FourSpaceManagerTest, GetFourSpaceIDTest)
{
  GetFourSpaceIDTest();
}
}   // namespace realcore
