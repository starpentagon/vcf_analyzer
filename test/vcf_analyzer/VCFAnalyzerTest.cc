#include "gtest/gtest.h"

#include "VCFTranspositionTable.h"
#include "VCFAnalyzer.h"

using namespace std;

namespace realcore
{

shared_ptr<VCFTable> vcf_table = make_shared<VCFTable>(0, kLockFree);

class VCFAnalyzerTest
: public ::testing::Test
{
public:
  void MakeMoveUndoTest(){

    MoveList move_list("hh");
    VCFAnalyzer vcf_analyzer(move_list, vcf_table);

    ASSERT_EQ(0, vcf_analyzer.search_sequence_.size());

    vcf_analyzer.MakeMove(kMoveHG);
    ASSERT_EQ(1, vcf_analyzer.search_sequence_.size());
    ASSERT_EQ(kMoveHG, vcf_analyzer.search_sequence_[0]);

    vcf_analyzer.UndoMove();
    ASSERT_EQ(0, vcf_analyzer.search_sequence_.size());
  }

  void GetCandidateMoveORTest()
  {
    {
      // 相手に四がある場合
      //   A B C D E F G H I J K L M N O 
      // A + --------------------------+ A 
      // B | . . . . . . . . . . . . . | B 
      // C | . . . . . . . . . . . . . | C 
      // D | . . * . . . . . . . * . . | D 
      // E | . . . . . . . . . . . . . | E 
      // F | . . . . . . . . . . . . . | F 
      // G | . . . . . o o . . . . . . | G 
      // H | . . . . . o x x x x . . . | H 
      // I | . . . . . . . . . . . . . | I 
      // J | . . . . . . . . . . . . . | J 
      // K | . . . . . . . . . . . . . | K 
      // L | . . * . . . . . . . * . . | L 
      // M | . . . . . . . . . . . . . | M 
      // N | . . . . . . . . . . . . . | N 
      // O + --------------------------+ O 
      //   A B C D E F G H I J K L M N O 
      MoveList move_list("hhhgihghjhggkh");
      VCFAnalyzer vcf_analyzer(move_list, vcf_table);

      VCFSearch vcf_search;
      vector<MovePair> candidate_move;
      vcf_analyzer.GetCandidateMoveOR<kWhiteTurn>(vcf_search, &candidate_move);

      ASSERT_EQ(0, candidate_move.size());
    }
    {
      // 相手に四がない場合
      //   A B C D E F G H I J K L M N O 
      // A + --------------------------+ A 
      // B | . . . . . . . . . . . . . | B 
      // C | . . . . . . . . . . . . . | C 
      // D | . . * . . . . . . . * . . | D 
      // E | . . . . . . . . . . . . . | E 
      // F | . . . . . . . . . . . . . | F 
      // G | . . . . . o o . . . . . . | G 
      // H | . . . . . o x x x . . . . | H 
      // I | . . . . . . . . . . . . . | I 
      // J | . . . . . . . . . . . . . | J 
      // K | . . . . . . . . . . . . . | K 
      // L | . . * . . . . . . . * . . | L 
      // M | . . . . . . . . . . . . . | M 
      // N | . . . . . . . . . . . . . | N 
      // O + --------------------------+ O 
      //   A B C D E F G H I J K L M N O 
      MoveList move_list("hhhgihghjhgg");
      VCFAnalyzer vcf_analyzer(move_list, vcf_table);

      VCFSearch vcf_search;
      vector<MovePair> candidate_move;
      vcf_analyzer.GetCandidateMoveOR<kBlackTurn>(vcf_search, &candidate_move);

      ASSERT_EQ(2, candidate_move.size());
    }
  }

  void GetSearchValueTest(){
    MoveList move_list("hhhgihghjhgg");
    VCFAnalyzer vcf_analyzer(move_list, vcf_table);

    ASSERT_EQ(kVCFStrongDisproved, vcf_analyzer.GetSearchValue(kVCFStrongDisproved));
    ASSERT_EQ(kVCFWeakDisprovedUB - 1, vcf_analyzer.GetSearchValue(kVCFWeakDisprovedUB));
    ASSERT_EQ(kVCFProvedUB - 1, vcf_analyzer.GetSearchValue(kVCFProvedUB));
  }
};

TEST_F(VCFAnalyzerTest, MakeMoveUndoTest){
  MakeMoveUndoTest();
}

TEST_F(VCFAnalyzerTest, GetCandidateMoveORTest){
  GetCandidateMoveORTest();
}

TEST_F(VCFAnalyzerTest, IsVCFProvedTest){
  ASSERT_FALSE(IsVCFProved(kVCFStrongDisproved));
  ASSERT_FALSE(IsVCFProved(kVCFWeakDisprovedLB));
  ASSERT_FALSE(IsVCFProved(kVCFWeakDisprovedUB));
  ASSERT_TRUE( IsVCFProved(kVCFProvedLB));
  ASSERT_TRUE( IsVCFProved(kVCFProvedUB));
}

TEST_F(VCFAnalyzerTest, IsVCFDisprovedTest){
  ASSERT_TRUE( IsVCFDisproved(kVCFStrongDisproved));
  ASSERT_FALSE(IsVCFDisproved(kVCFWeakDisprovedLB));
  ASSERT_FALSE(IsVCFDisproved(kVCFWeakDisprovedUB));
  ASSERT_FALSE(IsVCFDisproved(kVCFProvedLB));
  ASSERT_FALSE(IsVCFDisproved(kVCFProvedUB));
}

TEST_F(VCFAnalyzerTest, IsVCFWeakDisprovedTest){
  ASSERT_FALSE(IsVCFWeakDisproved(kVCFStrongDisproved));
  ASSERT_TRUE( IsVCFWeakDisproved(kVCFWeakDisprovedLB));
  ASSERT_TRUE( IsVCFWeakDisproved(kVCFWeakDisprovedUB));
  ASSERT_FALSE(IsVCFWeakDisproved(kVCFProvedLB));
  ASSERT_FALSE(IsVCFWeakDisproved(kVCFProvedUB));
}

TEST_F(VCFAnalyzerTest, GetVCFDepthTest){
  {
    constexpr VCFSearchValue value = kVCFProvedUB;
    ASSERT_EQ(1, GetVCFDepth(value));
  }
  {
    constexpr VCFSearchValue value = kVCFProvedLB;
    ASSERT_EQ(225, GetVCFDepth(value));
  }
  {
    constexpr VCFSearchValue value = kVCFWeakDisprovedUB;
    ASSERT_EQ(1, GetVCFDepth(value));
  }
  {
    constexpr VCFSearchValue value = kVCFWeakDisprovedLB;
    ASSERT_EQ(225, GetVCFDepth(value));
  }
}

TEST_F(VCFAnalyzerTest, GetVCFProvedSearchValueTest)
{
  {
    constexpr VCFSearchDepth depth = 1;
    ASSERT_EQ(kVCFProvedUB, GetVCFProvedSearchValue(depth));
  }
  {
    constexpr VCFSearchDepth depth = 225;
    ASSERT_EQ(kVCFProvedLB, GetVCFProvedSearchValue(depth));
  }
}

TEST_F(VCFAnalyzerTest, GetVCFWeakDisprovedSearchValueTest)
{
  {
    constexpr VCFSearchDepth depth = 1;
    ASSERT_EQ(kVCFWeakDisprovedUB, GetVCFWeakDisprovedSearchValue(depth));
  }
  {
    constexpr VCFSearchDepth depth = 225;
    ASSERT_EQ(kVCFWeakDisprovedLB, GetVCFWeakDisprovedSearchValue(depth));
  }
}

TEST_F(VCFAnalyzerTest, TranspositionTableTest)
{
  constexpr size_t table_space = 1;
  VCFTable vcf_table(table_space, kLockFree);

  static_assert(kVCFTableUseExactBoardInfo == 1, "This test assumes kVCFTableUseExactBoardInfo == 1");

  // Hash値は等しいが盤面情報が異なるケースのテスト
  constexpr HashValue hash_value = 0;
  BitBoard bit_board_1, bit_board_2;
  constexpr VCFSearchValue search_value = 1;

  bit_board_1.SetState<kBlackStone>(kMoveHH);
  bit_board_2.SetState<kBlackStone>(kMoveHG);

  vcf_table.Upsert(hash_value, bit_board_1, search_value);

  {
    VCFSearchValue table_value = 0;
    const auto find_result = vcf_table.find(hash_value, bit_board_1, &table_value);
    ASSERT_TRUE(find_result);
    ASSERT_EQ(search_value, table_value);
  }
  {
    VCFSearchValue table_value = 0;
    const auto find_result = vcf_table.find(hash_value, bit_board_2, &table_value);
    ASSERT_FALSE(find_result);
  }
}
TEST_F(VCFAnalyzerTest, GetSearchValueTest)
{
  GetSearchValueTest();
}

}
