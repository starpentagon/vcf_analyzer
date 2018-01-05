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
    MoveList move_list("hh");
    BitBoard bit_board(move_list);
    FourSpaceManager four_space_manager(bit_board);

    ASSERT_EQ(1, four_space_manager.four_space_list_.size());   // kInvalidFourSpaceIDに対応する要素を追加するためサイズは1
    ASSERT_TRUE(four_space_manager.four_space_hash_table_.empty());
    ASSERT_TRUE(four_space_manager.open_rest_key_puttable_four_space_id_.empty());
    ASSERT_TRUE(four_space_manager.open_rest_key_feasible_four_space_id_.empty());
    ASSERT_TRUE(four_space_manager.bit_board_ == bit_board);
  }

  void GetRegisterFourSpaceIDTest()
  {
    BitBoard bit_board;
    FourSpaceManager four_space_manager(bit_board);
    FourSpace empty_four_space;

    ASSERT_EQ(kInvalidFourSpaceID, four_space_manager.GetFourSpaceID(empty_four_space));

    FourSpace four_space_1(kMoveAA, kMoveAB);
    four_space_manager.RegisterFourSpace(four_space_1);

    ASSERT_NE(kInvalidFourSpaceID, four_space_manager.GetFourSpaceID(four_space_1));
    
    FourSpace four_space_2(kMoveAB, kMoveAA);
    ASSERT_EQ(kInvalidFourSpaceID, four_space_manager.GetFourSpaceID(four_space_2));
  }

  void GetRegisterOpenRestKeyFourSpaceTest()
  {
    BitBoard bit_board;
    {
      // 設置可能
      static constexpr OpenRestListKey key = kMoveAA;
      FourSpaceManager four_space_manager(bit_board);

      FourSpace four_space(kMoveAA, kMoveAB);
      const auto four_space_id = four_space_manager.RegisterFourSpace(four_space);
      bool is_registered = four_space_manager.RegisterOpenRestKeyFourSpace<kBlackTurn>(key, four_space_id);
      ASSERT_TRUE(is_registered);

      auto& puttable_id_list = four_space_manager.GetPuttableFourSpace(key);

      ASSERT_EQ(1, puttable_id_list.size());
      ASSERT_EQ(four_space_id, puttable_id_list[0]);

      const auto& feasible_id_list = four_space_manager.GetFeasibleFourSpace<kBlackTurn>(key);
      ASSERT_EQ(1, feasible_id_list.size());
      ASSERT_EQ(four_space_id, feasible_id_list[0]);

      is_registered = four_space_manager.RegisterOpenRestKeyFourSpace<kBlackTurn>(key, four_space_id);
      ASSERT_FALSE(is_registered);
    }
    {
      // 設置可能だが均等ではないケース
      static constexpr OpenRestListKey key = kMoveAA;
      FourSpaceManager four_space_manager(bit_board);

      FourSpace four_space(kMoveAA, kMoveAB);
      FourSpace unbalance(kMoveAA, kMoveAC);
      four_space.Add(unbalance);

      const auto four_space_id = four_space_manager.RegisterFourSpace(four_space);
      bool is_registered = four_space_manager.RegisterOpenRestKeyFourSpace<kBlackTurn>(key, four_space_id);
      ASSERT_FALSE(is_registered);

      auto& puttable_id_list = four_space_manager.GetPuttableFourSpace(key);

      ASSERT_EQ(1, puttable_id_list.size());
      ASSERT_EQ(four_space_id, puttable_id_list[0]);

      const auto& feasible_id_list = four_space_manager.GetFeasibleFourSpace<kBlackTurn>(key);
      ASSERT_EQ(0, feasible_id_list.size());
    }
    {
      // 設置可能だが五連以上が生じるケース
      static constexpr OpenRestListKey key = kMoveAA;
      FourSpaceManager four_space_manager(bit_board);

      FourSpace four_space(kMoveAA, kMoveAB);
      four_space.Add(kMoveBA, kMoveAC);
      four_space.Add(kMoveHH, kMoveAD);
      four_space.Add(kMoveHI, kMoveAE);
      four_space.Add(kMoveHJ, kMoveAF);

      const auto four_space_id = four_space_manager.RegisterFourSpace(four_space);
      bool is_registered = four_space_manager.RegisterOpenRestKeyFourSpace<kBlackTurn>(key, four_space_id);
      ASSERT_FALSE(is_registered);

      auto& puttable_id_list = four_space_manager.GetPuttableFourSpace(key);

      ASSERT_EQ(1, puttable_id_list.size());
      ASSERT_EQ(four_space_id, puttable_id_list[0]);

      const auto& feasible_id_list = four_space_manager.GetFeasibleFourSpace<kBlackTurn>(key);
      ASSERT_EQ(0, feasible_id_list.size());
    }
  }

  void GeneratePuttableFourSpaceTest()
  {
    BitBoard bit_board;
    FourSpaceManager four_space_manager(bit_board);

    // ID:1-1
    {
      FourSpace four_space_1_1(kMoveAA, kMoveAB);
      four_space_manager.RegisterFourSpace(four_space_1_1);
    }

    // ID:1-2
    {
      FourSpace four_space_1_2(kMoveOO, kMoveAA);
      four_space_manager.RegisterFourSpace(four_space_1_2);
    }

    // ID:2-1
    {
      FourSpace four_space_2_1(kMoveAA, kMoveAB);
      four_space_2_1.Add(kMoveAC, kMoveAD);
      four_space_manager.RegisterFourSpace(four_space_2_1);
    }

    // ID:2-2
    {
      FourSpace four_space_2_2(kMoveHH, kMoveHI);
      four_space_manager.RegisterFourSpace(four_space_2_2);
    }

    // ID:2-3
    {
      FourSpace four_space_2_3(kMoveBA, kMoveAB);
      four_space_2_3.Add(kMoveCA, kMoveAC);
      four_space_2_3.Add(kMoveDA, kMoveAD);
      four_space_2_3.Add(kMoveEA, kMoveAE);
      four_space_manager.RegisterFourSpace(four_space_2_3);
    }

    vector<FourSpaceID> id_list_1{1, 2}, id_list_2{3, 4, 5};
    vector<FourSpaceID> generated_list;

    four_space_manager.GeneratePuttableFourSpace(id_list_1, id_list_2, &generated_list);

    // 2 * 3 = 6通り中、(1-2)-(2-1)は同時設置不可のため除外
    ASSERT_EQ(5, generated_list.size());

    {
      // (1-1)-(2-1)
      FourSpace expect(kMoveAA, kMoveAB);
      expect.Add(kMoveAC, kMoveAD);

      const auto four_space_id = generated_list[0];
      const auto& four_space = four_space_manager.GetFourSpace(four_space_id);

      ASSERT_TRUE(expect == four_space);
    }
    {
      // (1-1)-(2-2)
      FourSpace expect(kMoveAA, kMoveAB);
      expect.Add(kMoveHH, kMoveHI);

      const auto four_space_id = generated_list[1];
      const auto& four_space = four_space_manager.GetFourSpace(four_space_id);

      ASSERT_TRUE(expect == four_space);
    }
    {
      // (1-1)-(2-3)
      FourSpace expect(kMoveAA, kMoveAB);
      FourSpace expect_sub;
      expect_sub.Add(kMoveBA, kMoveAB);
      expect_sub.Add(kMoveCA, kMoveAC);
      expect_sub.Add(kMoveDA, kMoveAD);
      expect_sub.Add(kMoveEA, kMoveAE);
      expect.Add(expect_sub);

      const auto four_space_id = generated_list[2];
      const auto& four_space = four_space_manager.GetFourSpace(four_space_id);

      ASSERT_TRUE(expect == four_space);
    }
    {
      // (1-2)-(2-2)
      FourSpace expect(kMoveOO, kMoveAA);
      expect.Add(kMoveHH, kMoveHI);

      const auto four_space_id = generated_list[3];
      const auto& four_space = four_space_manager.GetFourSpace(four_space_id);

      ASSERT_TRUE(expect == four_space);
    }
    {
      // (1-2)-(2-3)
      FourSpace expect(kMoveOO, kMoveAA);
      expect.Add(kMoveBA, kMoveAB);
      expect.Add(kMoveCA, kMoveAC);
      expect.Add(kMoveDA, kMoveAD);
      expect.Add(kMoveEA, kMoveAE);

      const auto four_space_id = generated_list[4];
      const auto& four_space = four_space_manager.GetFourSpace(four_space_id);

      ASSERT_TRUE(expect == four_space);
    }
  }

  void AddGetMultiElementTest()
  {
    BitBoard bit_board;
    FourSpaceManager four_space_manager(bit_board);
    
    {
      // kMoveAA-1
      FourSpace four_space(kMoveAA, kMoveBA);
      vector<RestKeyFourSpace> added_list;

      four_space_manager.AddFourSpace<kBlackTurn>(kMoveAA, kMoveBA, four_space, &added_list);
    }
    {
      // kMoveAA-2
      FourSpace four_space(kMoveAA, kMoveAB);
      vector<RestKeyFourSpace> added_list;

      four_space_manager.AddFourSpace<kBlackTurn>(kMoveAA, kMoveAB, four_space, &added_list);
    }
    {
      // kMoveAB-1
      FourSpace four_space(kMoveAB, kMoveBB);
      vector<RestKeyFourSpace> added_list;

      four_space_manager.AddFourSpace<kBlackTurn>(kMoveAB, kMoveBB, four_space, &added_list);
    }
    {
      // kMoveAB-2
      FourSpace four_space(kMoveAB, kMoveAA);
      vector<RestKeyFourSpace> added_list;

      four_space_manager.AddFourSpace<kBlackTurn>(kMoveAB, kMoveAA, four_space, &added_list);
    }

    vector<MovePosition> open_rest_move{kMoveAA, kMoveAB};
    OpenRestList open_rest_list(open_rest_move);
    const auto rest_key = open_rest_list.GetOpenRestKey();
    four_space_manager.AddOpenRestListKey(rest_key);
    
    {
      // rest = kMoveAA
      const auto& four_space_id_list = four_space_manager.GetFeasibleFourSpace<kBlackTurn>(kMoveAA);
      ASSERT_EQ(2, four_space_id_list.size());

      {
        const auto four_space_id = four_space_id_list[0];
        const auto& four_space = four_space_manager.GetFourSpace(four_space_id);
        FourSpace expect(kMoveAA, kMoveBA);
        ASSERT_TRUE(expect == four_space);
      }
      {
        const auto four_space_id = four_space_id_list[1];
        const auto& four_space = four_space_manager.GetFourSpace(four_space_id);
        FourSpace expect(kMoveAA, kMoveAB);
        ASSERT_TRUE(expect == four_space);
      }
    }
    {
      // rest = kMoveAB
      const auto& four_space_id_list = four_space_manager.GetFeasibleFourSpace<kBlackTurn>(kMoveAB);
      ASSERT_EQ(2, four_space_id_list.size());

      {
        const auto four_space_id = four_space_id_list[0];
        const auto& four_space = four_space_manager.GetFourSpace(four_space_id);
        FourSpace expect(kMoveAB, kMoveBB);
        ASSERT_TRUE(expect == four_space);
      }
      {
        const auto four_space_id = four_space_id_list[1];
        const auto& four_space = four_space_manager.GetFourSpace(four_space_id);
        FourSpace expect(kMoveAB, kMoveAA);
        ASSERT_TRUE(expect == four_space);
      }
    }
    {
      // rest = kMoveAA & kMoveAB
      const auto& four_space_id_list = four_space_manager.GetFeasibleFourSpace<kBlackTurn>(rest_key);
      ASSERT_EQ(1, four_space_id_list.size());

      {
        const auto four_space_id = four_space_id_list[0];
        const auto& four_space = four_space_manager.GetFourSpace(four_space_id);
        FourSpace expect(kMoveAA, kMoveBA);
        expect.Add(kMoveAB, kMoveBB);
        ASSERT_TRUE(expect == four_space);
      }
    }
  }

  void GetParentOpenRestListKeyTest()
  {
    {
      BitBoard bit_board;
      FourSpaceManager four_space_manager(bit_board);

      four_space_manager.open_rest_key_puttable_four_space_id_.insert(make_pair(kMoveAA, vector<FourSpaceID>(1)));
      four_space_manager.open_rest_key_puttable_four_space_id_.insert(make_pair(kMoveAC, vector<FourSpaceID>(2)));

      const OpenRestListKey parent_key = (kMoveAA << 8) | kMoveAC;
      four_space_manager.open_rest_key_puttable_four_space_id_.insert(make_pair(parent_key, vector<FourSpaceID>(3)));

      const OpenRestListKey key = (kMoveAA << 16) | (kMoveAB << 8) | kMoveAC;
      four_space_manager.open_rest_key_puttable_four_space_id_.insert(make_pair(key, vector<FourSpaceID>(4)));

      {
      const auto parent_info = four_space_manager.GetParentOpenRestListKey(key);
      ASSERT_EQ(kMoveAB, get<0>(parent_info));
      ASSERT_EQ(parent_key, get<1>(parent_info));
      ASSERT_EQ(true, get<2>(parent_info));
      }
      {
      const auto parent_info = four_space_manager.GetParentOpenRestListKey(kMoveAA);
      ASSERT_EQ(kNullMove, get<0>(parent_info));
      ASSERT_EQ(kMoveAA, get<1>(parent_info));
      ASSERT_EQ(true, get<2>(parent_info));
      }
      {
      const OpenRestListKey nonregistered_key = (kMoveHA << 16) | (kMoveHB << 8) | kMoveHC;
      const OpenRestListKey expect_parent_key = (kMoveHB << 8) | kMoveHC;
      const auto parent_info = four_space_manager.GetParentOpenRestListKey(nonregistered_key);
      ASSERT_EQ(kMoveHA, get<0>(parent_info));
      ASSERT_EQ(expect_parent_key, get<1>(parent_info));
      ASSERT_EQ(false, get<2>(parent_info));
      }
    }
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
  
  BitBoard bit_board;
  FourSpaceManager four_space_manager(bit_board);
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

TEST_F(FourSpaceManagerTest, AddGetMultiElementTest)
{
  AddGetMultiElementTest();
}

TEST_F(FourSpaceManagerTest, GetRegisterFourSpaceIDTest)
{
  GetRegisterFourSpaceIDTest();
}

TEST_F(FourSpaceManagerTest, GetRegisterOpenRestKeyFourSpaceTest)
{
  GetRegisterOpenRestKeyFourSpaceTest();
}

TEST_F(FourSpaceManagerTest, GeneratePuttableFourSpaceTest)
{
  GeneratePuttableFourSpaceTest();
}

TEST_F(FourSpaceManagerTest, GetMaxRelaxedFourLengthTest)
{
  BitBoard bit_board;
  FourSpaceManager four_space_manager(bit_board);
  
  ASSERT_EQ(0, four_space_manager.GetMaxRelaxedFourLength());

  {
    FourSpace four_space(kMoveAA, kMoveAB);
    vector<RestKeyFourSpace> added_list;
    
    four_space_manager.AddFourSpace<kBlackTurn>(kMoveAA, kMoveAB, four_space, &added_list);
    ASSERT_EQ(1, four_space_manager.GetMaxRelaxedFourLength());
  }
  {
    FourSpace four_space(kMoveAA, kMoveAB);
    four_space.Add(kMoveAC, kMoveAD);
    four_space.Add(kMoveAE, kMoveAF);
    four_space.Add(kMoveAG, kMoveAH);
    vector<RestKeyFourSpace> added_list;
    
    four_space_manager.AddFourSpace<kBlackTurn>(kMoveAA, kMoveAB, four_space, &added_list);
    ASSERT_EQ(4, four_space_manager.GetMaxRelaxedFourLength());
  }
}

TEST_F(FourSpaceManagerTest, GetFourSpaceCountTest)
{
  BitBoard bit_board;
  FourSpaceManager four_space_manager(bit_board);
  
  for(const auto move : GetAllMove()){
    ASSERT_EQ(0, four_space_manager.GetFourSpaceCount(move));
  }

  {
    FourSpace four_space(kMoveAA, kMoveAB);
    vector<RestKeyFourSpace> added_list;
    
    four_space_manager.AddFourSpace<kBlackTurn>(kMoveAA, kMoveAB, four_space, &added_list);

    for(const auto move : GetAllMove()){
      if(move == kMoveAA){
        ASSERT_EQ(1, four_space_manager.GetFourSpaceCount(move));
      }else{
        ASSERT_EQ(0, four_space_manager.GetFourSpaceCount(move));
      }
    }
  }
  {
    FourSpace four_space(kMoveAA, kMoveAB);
    four_space.Add(kMoveAC, kMoveAD);
    four_space.Add(kMoveAE, kMoveAF);
    four_space.Add(kMoveAG, kMoveAH);
    vector<RestKeyFourSpace> added_list;
    
    four_space_manager.AddFourSpace<kBlackTurn>(kMoveAA, kMoveAB, four_space, &added_list);

    for(const auto move : GetAllMove()){
      if(move == kMoveAA){
        ASSERT_EQ(2, four_space_manager.GetFourSpaceCount(move));
      }else{
        ASSERT_EQ(0, four_space_manager.GetFourSpaceCount(move));
      }
    }
  }
}

TEST_F(FourSpaceManagerTest, GetParentOpenRestListKeyTest)
{
  GetParentOpenRestListKeyTest();
}
}   // namespace realcore
