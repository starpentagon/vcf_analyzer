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
    FourSpaceManager four_space_manager;

    ASSERT_EQ(1, four_space_manager.four_space_list_.size());   // kInvalidFourSpaceIDに対応する要素を追加するためサイズは1
    ASSERT_TRUE(four_space_manager.open_rest_key_four_space_id_.empty());
  }

  void GetFourSpaceIDTest()
  {
    FourSpaceManager four_space_manager;
    FourSpace empty_four_space;

    ASSERT_EQ(kInvalidFourSpaceID, four_space_manager.GetFourSpaceID(kMoveAA, empty_four_space));

    FourSpace four_space_1(kMoveAA, kMoveAB);
    vector<RestKeyFourSpace> added_list;
    four_space_manager.AddFourSpace(kMoveAA, kMoveAB, four_space_1, &added_list);

    ASSERT_EQ(1, four_space_manager.GetFourSpaceID(kMoveAA, four_space_1));
    
    FourSpace four_space_2(kMoveAB, kMoveAA);
    ASSERT_EQ(kInvalidFourSpaceID, four_space_manager.GetFourSpaceID(kMoveAA, four_space_2));
    ASSERT_EQ(kInvalidFourSpaceID, four_space_manager.GetFourSpaceID(kMoveAB, four_space_1));
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
  
  FourSpaceManager four_space_manager;
  vector<RestKeyFourSpace> added_list;

  four_space_manager.AddFourSpace(gain, cost, four_space, &added_list);

  ASSERT_EQ(1, added_list.size());

  const auto rest_key = added_list[0].first;
  const auto four_space_id = added_list[0].second;

  ASSERT_EQ(rest_key, 0);
  ASSERT_EQ(1, four_space_id);

  const auto &manager_four_space = four_space_manager.GetFourSpace(four_space_id);
  ASSERT_TRUE(manager_four_space == four_space);
}

TEST_F(FourSpaceManagerTest, GetFourSpaceIDTest)
{
  GetFourSpaceIDTest();
}
}   // namespace realcore
