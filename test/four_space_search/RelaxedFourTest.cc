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
  void CanGuardOpponentFourTest()
  {
    vector<MovePosition> rest_list;

  {
    // 正例
    MoveList move_list("hhdafacaebbaecef");
    BitBoard bit_board(move_list);
    FourSpace four_space(kMoveED, kMoveEG);

    RelaxedFour relaxed_four(kMoveEE, kMoveEA, rest_list);
    MovePair move_pair(kMoveEA, kMoveAA);
    const bool can_guard = relaxed_four.CanGuardOpponentFour<kBlackTurn>(four_space, move_pair, bit_board);

    ASSERT_TRUE(can_guard);
  }
  {
    // 負例
    MoveList move_list("hhdafacaebbaecefabacbbcc");
    BitBoard bit_board(move_list);
    FourSpace four_space(kMoveED, kMoveEG);

    RelaxedFour relaxed_four(kMoveEE, kMoveEA, rest_list);
    MovePair move_pair(kMoveEA, kMoveAA);
    const bool can_guard = relaxed_four.CanGuardOpponentFour<kBlackTurn>(four_space, move_pair, bit_board);

    ASSERT_FALSE(can_guard);
  }
  }

  void CheckTranspositionTableTest()
  {
    vector<MovePosition> rest_list;
    MovePair opponent_four;
    
    MoveList move_list("hhhgihgh");
    BitBoard bit_board(move_list);
    FourSpace four_space(kMoveJH, kMoveHI);

    RelaxedFour relaxed_four(kMoveKH, kMoveLH, rest_list);

    ASSERT_TRUE(relaxed_four.transposition_table_.empty());
    const auto status = relaxed_four.GetRelaxedFourStatus<kBlackTurn>(four_space, bit_board, &opponent_four);

    ASSERT_EQ(1, relaxed_four.transposition_table_.size());
    const auto table_data = relaxed_four.transposition_table_[0];

    MoveBitSet local_bit_set = GetLineNeighborhoodBit<5>(kMoveKH) | GetLineNeighborhoodBit<5>(kMoveLH);
    FourSpace local_four_space(four_space, local_bit_set);

    ASSERT_EQ(local_four_space, table_data.first);
    ASSERT_EQ(status, table_data.second);

    ASSERT_EQ(status, relaxed_four.CheckTranspositionTable(local_four_space));
  }
};

TEST_F(RelaxedFourTest, ConstructorTest)
{
  vector<MovePosition> rest_list{{kMoveAD, kMoveAC}};
  RelaxedFour relaxed_four(kMoveAA, kMoveAB, rest_list);

  ASSERT_EQ(kMoveAA, relaxed_four.GetGainPosition());
  ASSERT_EQ(kMoveAB, relaxed_four.GetCostPosition());
  ASSERT_EQ(rest_list, relaxed_four.GetRestPositionList());
  ASSERT_TRUE(relaxed_four.GetTranspositionTable().empty());
}

TEST_F(RelaxedFourTest, CompareOperTest)
{
  {
    // 等しいケース
    vector<MovePosition> rest_list_1{{kMoveAD, kMoveAC}};
    RelaxedFour relaxed_four_1(kMoveAA, kMoveAB, rest_list_1);

    vector<MovePosition> rest_list_2{{kMoveAC, kMoveAD}};
    RelaxedFour relaxed_four_2(kMoveAA, kMoveAB, rest_list_2);
    ASSERT_TRUE(relaxed_four_1 == relaxed_four_2);
  }
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
    vector<MovePosition> rest_list{{kMoveAD, kMoveAE, kMoveAC}};
    RelaxedFour relaxed_four(kMoveAA, kMoveAB, rest_list);
    constexpr uint64_t key = static_cast<uint64_t>(kMoveAA) << 32 | kMoveAB << 24 | kMoveAE << 16 | kMoveAD << 8 | kMoveAC;

    ASSERT_EQ(key, relaxed_four.GetKey());
  }
  {
    vector<MovePosition> rest_list{{kMoveAD, kMoveAC}};
    RelaxedFour relaxed_four(kMoveAA, kMoveAB, rest_list);
    constexpr uint64_t key = static_cast<uint64_t>(kMoveAA) << 32 | kMoveAB << 24 | kMoveAD << 8 | kMoveAC;

    ASSERT_EQ(key, relaxed_four.GetKey());
  }
  {
    vector<MovePosition> rest_list{kMoveAC};
    RelaxedFour relaxed_four(kMoveAA, kMoveAB, rest_list);
    constexpr uint64_t key = static_cast<uint64_t>(kMoveAA) << 32 | kMoveAB << 24 | kMoveAC;

    ASSERT_EQ(key, relaxed_four.GetKey());
  }
  {
    vector<MovePosition> rest_list;
    RelaxedFour relaxed_four(kMoveAA, kMoveAB, rest_list);
    constexpr uint64_t key = static_cast<uint64_t>(kMoveAA) << 32 | kMoveAB << 24;

    ASSERT_EQ(key, relaxed_four.GetKey());
  }
}

TEST_F(RelaxedFourTest, GetRelaxedFourStatusInfeasibleTest)
{
  vector<MovePosition> rest_list;
  MovePair opponent_four;

  {
    // 攻め方(黒)に五連 -> 実現不可能
    MoveList move_list("hhhihghjhfhk");
    BitBoard bit_board(move_list);
    FourSpace four_space(kMoveHE, kMoveHL);

    RelaxedFour relaxed_four(kMoveHD, kMoveHC, rest_list);
    const auto status = relaxed_four.GetRelaxedFourStatus<kBlackTurn>(four_space, bit_board, &opponent_four);

    ASSERT_EQ(kRelaxedFourInfeasible, status);
  }
  {
    // 攻め方(黒)に長連 -> 実現不可能
    MoveList move_list("hhhihghjhfhkhcoo");
    BitBoard bit_board(move_list);
    FourSpace four_space(kMoveHE, kMoveHL);

    RelaxedFour relaxed_four(kMoveHD, kMoveID, rest_list);
    const auto status = relaxed_four.GetRelaxedFourStatus<kBlackTurn>(four_space, bit_board, &opponent_four);

    ASSERT_EQ(kRelaxedFourInfeasible, status);
  }
  {
    // 攻め方(白)に五連 -> 実現不可能
    MoveList move_list("aahhhihghjhfhk");
    BitBoard bit_board(move_list);
    FourSpace four_space(kMoveHE, kMoveHL);

    RelaxedFour relaxed_four(kMoveHD, kMoveHC, rest_list);
    const auto status = relaxed_four.GetRelaxedFourStatus<kWhiteTurn>(four_space, bit_board, &opponent_four);

    ASSERT_EQ(kRelaxedFourInfeasible, status);
  }
  {
    // 攻め方(白)に長連 -> 実現不可能
    MoveList move_list("aahhhihghjhfhkhcoo");
    BitBoard bit_board(move_list);
    FourSpace four_space(kMoveHE, kMoveHL);

    RelaxedFour relaxed_four(kMoveHD, kMoveID, rest_list);
    const auto status = relaxed_four.GetRelaxedFourStatus<kWhiteTurn>(four_space, bit_board, &opponent_four);

    ASSERT_EQ(kRelaxedFourInfeasible, status);
  }
  {
    // 受け方(白)に五連 -> 実現不可能
    MoveList move_list("hhhihghjhfhk");
    BitBoard bit_board(move_list);
    FourSpace four_space(kMoveHE, kMoveHL);

    RelaxedFour relaxed_four(kMoveAA, kMoveHM, rest_list);
    const auto status = relaxed_four.GetRelaxedFourStatus<kBlackTurn>(four_space, bit_board, &opponent_four);

    ASSERT_EQ(kRelaxedFourInfeasible, status);
  }
  {
    // 受け方(白)に長連 -> 実現不可能
    MoveList move_list("hhhihghjhfhkaahn");
    BitBoard bit_board(move_list);
    FourSpace four_space(kMoveHE, kMoveHL);

    RelaxedFour relaxed_four(kMoveAB, kMoveHM, rest_list);
    const auto status = relaxed_four.GetRelaxedFourStatus<kBlackTurn>(four_space, bit_board, &opponent_four);

    ASSERT_EQ(kRelaxedFourInfeasible, status);
  }
  {
    // 受け方(黒)に五連 -> 実現不可能
    MoveList move_list("aahhhihghjhfhk");
    BitBoard bit_board(move_list);
    FourSpace four_space(kMoveHE, kMoveHL);

    RelaxedFour relaxed_four(kMoveHD, kMoveHM, rest_list);
    const auto status = relaxed_four.GetRelaxedFourStatus<kWhiteTurn>(four_space, bit_board, &opponent_four);

    ASSERT_EQ(kRelaxedFourInfeasible, status);
  }
  {
    // ノリ返しが不可能な四ノリが発生 -> 実現不可能
    MoveList move_list("hhdafacaebbaecefabacbbcc");
    BitBoard bit_board(move_list);
    FourSpace four_space(kMoveED, kMoveEG);

    RelaxedFour relaxed_four(kMoveEE, kMoveEA, rest_list);
    const auto status = relaxed_four.GetRelaxedFourStatus<kBlackTurn>(four_space, bit_board, &opponent_four);
    
    ASSERT_EQ(kRelaxedFourInfeasible, status);
  }
}

TEST_F(RelaxedFourTest, GetRelaxedFourStatusFourFailTest)
{
  vector<MovePosition> rest_list;
  MovePair opponent_four;

  {
    // 攻め方(黒)の四が成立しない -> 四ノビ失敗
    MoveList move_list("hhhihghjhchk");
    BitBoard bit_board(move_list);
    FourSpace four_space(kMoveHD, kMoveHL);

    RelaxedFour relaxed_four(kMoveHE, kMoveAA, rest_list);
    const auto status = relaxed_four.GetRelaxedFourStatus<kBlackTurn>(four_space, bit_board, &opponent_four);

    ASSERT_EQ(kRelaxedFourFail, status);
  }
}

TEST_F(RelaxedFourTest, GetRelaxedFourStatusFourDblFourThreeTest)
{
  vector<MovePosition> rest_list;
  MovePair opponent_four;
  
  {
    // 攻め方(黒)が四々 -> 四々/三々
    MoveList move_list("hhhihghjhehckaffjbjf");
    BitBoard bit_board(move_list);
    FourSpace four_space(kMoveIC, kMoveAA);

    RelaxedFour relaxed_four(kMoveHD, kMoveAB, rest_list);
    const auto status = relaxed_four.GetRelaxedFourStatus<kBlackTurn>(four_space, bit_board, &opponent_four);

    ASSERT_EQ(kRelaxedFourDblFourThree, status);
  }
  {
    // 攻め方(黒)が三々 -> 四々/三々
    MoveList move_list("hhhiihkhifdh");
    BitBoard bit_board(move_list);
    FourSpace four_space(kMoveJE, kMoveAA);

    RelaxedFour relaxed_four(kMoveGH, kMoveAB, rest_list);
    const auto status = relaxed_four.GetRelaxedFourStatus<kBlackTurn>(four_space, bit_board, &opponent_four);

    ASSERT_EQ(kRelaxedFourDblFourThree, status);
  }
  {
    // 攻め方(白)が四々 -> 黒番のみなので四々/三々ではない
    MoveList move_list("oohhhihghjhehckaffjbjf");
    BitBoard bit_board(move_list);
    FourSpace four_space(kMoveIC, kMoveAA);

    RelaxedFour relaxed_four(kMoveHD, kMoveAB, rest_list);
    const auto status = relaxed_four.GetRelaxedFourStatus<kWhiteTurn>(four_space, bit_board, &opponent_four);

    ASSERT_NE(kRelaxedFourDblFourThree, status);
  }
  {
    // 攻め方(白)が三々 -> 黒番のみなので四々/三々ではない
    MoveList move_list("oohhhiihkhifdhggiiehao");
    BitBoard bit_board(move_list);
    FourSpace four_space(kMoveGI, kMoveAA);

    RelaxedFour relaxed_four(kMoveGH, kMoveFH, rest_list);
    const auto status = relaxed_four.GetRelaxedFourStatus<kWhiteTurn>(four_space, bit_board, &opponent_four);

    ASSERT_NE(kRelaxedFourDblFourThree, status);
  }
}

TEST_F(RelaxedFourTest, GetRelaxedFourStatusFourTerminateTest)
{
  vector<MovePosition> rest_list;
  MovePair opponent_four;
  
  {
    // 攻め方(白)が長連に極める -> 終端
    MoveList move_list("hhhihcgdhgebhf");
    BitBoard bit_board(move_list);
    FourSpace four_space(kMoveIF, kMoveHD);

    RelaxedFour relaxed_four(kMoveFC, kMoveHE, rest_list);
    const auto status = relaxed_four.GetRelaxedFourStatus<kWhiteTurn>(four_space, bit_board, &opponent_four);

    ASSERT_EQ(kRelaxedFourTerminate, status);
  }
  {
    // 攻め方(黒)が四連を作る -> 終端
    MoveList move_list("hhhiihii");
    BitBoard bit_board(move_list);
    FourSpace four_space(kMoveJH, kMoveJI);

    RelaxedFour relaxed_four(kMoveKH, kMoveKI, rest_list);
    const auto status = relaxed_four.GetRelaxedFourStatus<kBlackTurn>(four_space, bit_board, &opponent_four);

    ASSERT_EQ(kRelaxedFourTerminate, status);
  }
  {
    // 攻め方(白)が四連を作る -> 終端
    MoveList move_list("aahhhiihii");
    BitBoard bit_board(move_list);
    FourSpace four_space(kMoveJH, kMoveJI);

    RelaxedFour relaxed_four(kMoveKH, kMoveKI, rest_list);
    const auto status = relaxed_four.GetRelaxedFourStatus<kWhiteTurn>(four_space, bit_board, &opponent_four);

    ASSERT_EQ(kRelaxedFourTerminate, status);
  }
  {
    // 攻め方(白)が四々を作る -> 終端
    MoveList move_list("hhhghihfhjheoofaaogbho");
    BitBoard bit_board(move_list);
    FourSpace four_space(kMoveID, kMoveKF);

    RelaxedFour relaxed_four(kMoveHC, kMoveJE, rest_list);
    const auto status = relaxed_four.GetRelaxedFourStatus<kWhiteTurn>(four_space, bit_board, &opponent_four);

    ASSERT_EQ(kRelaxedFourTerminate, status);
  }
  {
    // 攻め方(白)が四々に極める -> 終端
    MoveList move_list("hhhghihfhjhegmimfnjn");
    BitBoard bit_board(move_list);
    FourSpace four_space(kMoveKO, kMoveEO);

    RelaxedFour relaxed_four(kMoveGK, kMoveHL, rest_list);
    const auto status = relaxed_four.GetRelaxedFourStatus<kWhiteTurn>(four_space, bit_board, &opponent_four);

    ASSERT_EQ(kRelaxedFourTerminate, status);
  }
  {
    // 攻め方(白)が見かけの三々に極める -> 終端
    MoveList move_list("hhhgjhjiigjfaa");
    BitBoard bit_board(move_list);
    FourSpace four_space(kMoveLK, kMoveII);

    RelaxedFour relaxed_four(kMoveKJ, kMoveIH, rest_list);
    const auto status = relaxed_four.GetRelaxedFourStatus<kWhiteTurn>(four_space, bit_board, &opponent_four);

    ASSERT_EQ(kRelaxedFourTerminate, status);
  }
}

TEST_F(RelaxedFourTest, GetRelaxedFourStatusFourOpponentFourTest)
{
  vector<MovePosition> rest_list;
  MovePair opponent_four;
  
  {
    // ノリ返しの可能性がある四ノリが発生
    MoveList move_list("hhdafacaebbaecef");
    BitBoard bit_board(move_list);
    FourSpace four_space(kMoveED, kMoveEG);
    
    RelaxedFour relaxed_four(kMoveEE, kMoveEA, rest_list);
    const auto status = relaxed_four.GetRelaxedFourStatus<kBlackTurn>(four_space, bit_board, &opponent_four);

    ASSERT_EQ(kRelaxedFourOpponentFour, status);
  }
}

TEST_F(RelaxedFourTest, GetRelaxedFourStatusFourFeasibleTest)
{
  vector<MovePosition> rest_list;
  MovePair opponent_four;
  
  {
    MoveList move_list("hhhgihgh");
    BitBoard bit_board(move_list);
    FourSpace four_space(kMoveJH, kMoveHI);

    RelaxedFour relaxed_four(kMoveKH, kMoveLH, rest_list);
    const auto status = relaxed_four.GetRelaxedFourStatus<kBlackTurn>(four_space, bit_board, &opponent_four);

    ASSERT_EQ(kRelaxedFourFeasible, status);
  }
}

TEST_F(RelaxedFourTest, CanGuardOpponentFourTest)
{
  CanGuardOpponentFourTest();
}

TEST_F(RelaxedFourTest, CheckTranspositionTableTest)
{
  CheckTranspositionTableTest();
}

TEST_F(RelaxedFourTest, IsExpandableTest)
{
  vector<MovePosition> rest_list;
  RelaxedFour relaxed_four(kMoveAA, kMoveAB, rest_list);

  ASSERT_FALSE(relaxed_four.IsExpandable(kRelaxedFourInfeasible));
  ASSERT_FALSE(relaxed_four.IsExpandable(kRelaxedFourFail));
  ASSERT_TRUE(relaxed_four.IsExpandable(kRelaxedFourDblFourThree));
  ASSERT_FALSE(relaxed_four.IsExpandable(kRelaxedFourTerminate));
  ASSERT_TRUE(relaxed_four.IsExpandable(kRelaxedFourOpponentFour));
  ASSERT_TRUE(relaxed_four.IsExpandable(kRelaxedFourFeasible));
}

}