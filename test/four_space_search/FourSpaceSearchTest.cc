#include "gtest/gtest.h"

#include "FourSpaceSearch.h"

using namespace std;

namespace realcore
{
class FourSpaceSearchTest
: public ::testing::Test
{
public:
  void UpdateSingleFourBlack()
  {
    //   A B C D E F G H I J K L M N O 
    // A + --------------------------o A 
    // B | . . . . . . . . . . . . . | B 
    // C | . . . . . . . . . . . . . | C 
    // D | . . * . . . . . . . * . . | D 
    // E | . . . . . . . . . . . . . | E 
    // F | . . . . . . . . . . . . . | F 
    // G | . . . . . . . . . . . . . | G 
    // H | . . . . . . x . . . . . . | H 
    // I | . . . . . . x . . . . . . | I 
    // J | . . . . . . x . . . . . . | J 
    // K | . . . . . . o . . . . . . | K 
    // L | . . * . . . . . . . * . . | L 
    // M | . . . . . . . . . . . . . | M 
    // N | . . . . . . . . . . . . . | N 
    // O + --------------------------o O 
    //   A B C D E F G H I J K L M N O 
    MoveList move_list("hhoahihkhjoo");
    BitBoard bit_board(move_list);

    FourSpaceSearch four_space_search(bit_board);
    four_space_search.ExpandFourSpace<kBlackTurn>();

    const auto &reach_region = four_space_search.reach_region_;
    const auto &put_region = four_space_search.put_region_;

    const MovePosition expect_list[] = {
      kMoveHF, kMoveHG
    };

    MoveBitSet expect_bit;

    for(const auto move : expect_list){
      expect_bit.set(move);
    }

    for(const auto move : GetAllInBoardMove()){
      EXPECT_EQ(expect_bit[move], !reach_region[move].empty());
      EXPECT_EQ(expect_bit[move], !put_region[move].empty());
    }
  }

  void UpdateSingleFourWhite()
  {
    //   A B C D E F G H I J K L M N O 
    // A x --------------------------x A 
    // B | . . . . . . . . . . . . . | B 
    // C | . . . . . . . . . . . . . | C 
    // D | . . * . . . . . . . * . . | D 
    // E | . . . . . . . . . . . . . | E 
    // F | . . . . . . . . . . . . . | F 
    // G | . . . . . x o o . o . . . | G 
    // H | . . . . . . x . . . . . . | H 
    // I | . . . . . . . . . . . . . | I 
    // J | . . . . . . . . . . . . . | J 
    // K | . . . . . . . . . . . . . | K 
    // L | . . * . . . . . . . * . . | L 
    // M | . . . . . . . . . . . . . | M 
    // N | . . . . . . . . . . . . . | N 
    // O + --------------------------+ O 
    //   A B C D E F G H I J K L M N O 
    MoveList move_list("hhhgggigoakgaa");
    BitBoard bit_board(move_list);

    FourSpaceSearch four_space_search(bit_board);
    four_space_search.ExpandFourSpace<kWhiteTurn>();

    const auto &reach_region = four_space_search.reach_region_;
    const auto &put_region = four_space_search.put_region_;

    const MovePosition expect_list[] = {
      kMoveJG, kMoveLG
    };

    MoveBitSet expect_bit;

    for(const auto move : expect_list){
      expect_bit.set(move);
    }

    for(const auto move : GetAllInBoardMove()){
      EXPECT_EQ(expect_bit[move], !reach_region[move].empty());
      EXPECT_EQ(expect_bit[move], !put_region[move].empty());
    }
  }

  void UpdateExpandableFourBlack()
  {
    //   A B C D E F G H I J K L M N O 
    // A + --------------------------o A 
    // B | . . . . . . . . . . . . . | B 
    // C | . . . . . . . . . . . . . | C 
    // D | . . * . . . . . . . * . . | D 
    // E | . . . . . . . . . . . . . | E 
    // F | . . . . . . x . . . . . . | F 
    // G | . . . . . . . . . . . . . | G 
    // H | . . . . . . x . . . . . . | H 
    // I | . . . . . . . . . . . . . | I 
    // J | . . . . . . x . . . . . . | J 
    // K | . . . . . . o . . . . . . | K 
    // L | . . * . . . . . . . * . . | L 
    // M | . . . . . . . . . . . . . | M 
    // N | . . . . . . . . . . . . . | N 
    // O + --------------------------o O 
    //   A B C D E F G H I J K L M N O 
    MoveList move_list("hhoahjhkhfoo");
    BitBoard bit_board(move_list);

    FourSpaceSearch four_space_search(bit_board);
    four_space_search.ExpandFourSpace<kBlackTurn>();

    const auto &reach_region = four_space_search.reach_region_;
    const auto &put_region = four_space_search.put_region_;

    const MovePosition expect_list[] = {
      kMoveHD, kMoveHE, kMoveHG, kMoveHI
    };

    MoveBitSet expect_bit;

    for(const auto move : expect_list){
      expect_bit.set(move);
    }

    for(const auto move : GetAllInBoardMove()){
      if(expect_bit[move] == reach_region[move].empty()){
        cerr << MoveString(move) << endl;
      }

      EXPECT_EQ(expect_bit[move], !reach_region[move].empty());
      EXPECT_EQ(expect_bit[move], !put_region[move].empty());
    }
  }

  void UpdateExpandableFourWhite()
  {
    //   A B C D E F G H I J K L M N O 
    // A + --------------------------x A 
    // B | . . . . . . . . . . . . . | B 
    // C | . . . . . . . . . . . . . | C 
    // D | . . * . . . . . . . * . . | D 
    // E | . . . . . . . . . . . . . | E 
    // F | . . . . . . . . . . . . . | F 
    // G | . . . . . x o . o o . . . | G 
    // H | . . . . . . x . . . . . . | H 
    // I | . . . . . . . . . . . . . | I 
    // J | . . . . . . . . . . . . . | J 
    // K | . . . . . . . . . . . . . | K 
    // L | . . * . . . . . . . * . . | L 
    // M | . . . . . . . . . . . . . | M 
    // N | . . . . . . . . . . . . . | N 
    // O + --------------------------x O 
    //   A B C D E F G H I J K L M N O 
    MoveList move_list("hhhgggkgoojgoa");
    BitBoard bit_board(move_list);

    FourSpaceSearch four_space_search(bit_board);
    four_space_search.ExpandFourSpace<kWhiteTurn>();

    const auto &reach_region = four_space_search.reach_region_;
    const auto &put_region = four_space_search.put_region_;

    const MovePosition expect_list[] = {
      kMoveIG, kMoveLG, kMoveMG, kMoveNG
    };

    MoveBitSet expect_bit;

    for(const auto move : expect_list){
      expect_bit.set(move);
    }

    for(const auto move : GetAllInBoardMove()){
      EXPECT_EQ(expect_bit[move], !reach_region[move].empty());
      EXPECT_EQ(expect_bit[move], !put_region[move].empty());
    }
  }

  void UpdateMultiExpandableFourBlack()
  {
    //   A B C D E F G H I J K L M N O 
    // A o --o ----o --------o ------o A 
    // B | . . . . x . . . . . . . x | B 
    // C | . . . . x . . . . . . x . | C 
    // D | . . * . x . . . . . x . . | D 
    // E | . . . . . . . . . . . . . | E 
    // F | . . . . . . . . . . . . . | F 
    // G | . . . . . . . . . . . . . | G 
    // H | . . . . . . x . . . . . . | H 
    // I | . . . . . . x . . . . . . | I 
    // J | . . . . . . x . . . . . . | J 
    // K | . . . . . . o . . . . . . | K 
    // L | . . * . . . . . . . * . . | L 
    // M | . . . . . . . . . . . . . | M 
    // N | . . . . . . . . . . . . . | N 
    // O o ------------o ------------o O 
    //   A B C D E F G H I J K L M N O 
    MoveList move_list("hhaahihkhjoanbfafbcafcoofdhomckaldao");
    BitBoard bit_board(move_list);

    FourSpaceSearch four_space_search(bit_board);
    four_space_search.ExpandFourSpace<kBlackTurn>();

    const auto &reach_region = four_space_search.reach_region_;
    const auto &put_region = four_space_search.put_region_;

    const MovePosition expect_list[] = {
      kMoveFE, kMoveFF, kMoveHF, kMoveHG, 
      kMoveJF, kMoveKE, kMoveHF, kMoveHG, 
      kMoveDF, kMoveEF, kMoveGF, kMoveIF, kMoveKF, kMoveLF 
    };

    MoveBitSet expect_bit;

    for(const auto move : expect_list){
      expect_bit.set(move);
    }

    for(const auto move : GetAllInBoardMove()){
      if(expect_bit[move] == reach_region[move].empty()){
        cerr << MoveString(move) << endl;
      }

      EXPECT_EQ(expect_bit[move], !reach_region[move].empty());
      EXPECT_EQ(expect_bit[move], !put_region[move].empty());
    }
  }

};

TEST_F(FourSpaceSearchTest, UpdateSingleFourBlackTest)
{
  UpdateSingleFourBlack();
}

TEST_F(FourSpaceSearchTest, UpdateSingleFourWhiteTest)
{
  UpdateSingleFourWhite();
}

TEST_F(FourSpaceSearchTest, UpdateExpandableFourBlackTest)
{
  UpdateExpandableFourBlack();
}

TEST_F(FourSpaceSearchTest, UpdateExpandableFourWhiteTest)
{
  UpdateExpandableFourWhite();
}

TEST_F(FourSpaceSearchTest, UpdateMultiExpandableFourBlackTest)
{
  UpdateMultiExpandableFourBlack();
}
}
