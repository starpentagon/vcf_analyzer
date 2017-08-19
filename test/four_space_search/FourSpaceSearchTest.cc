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

    EXPECT_EQ(2, four_space_search.GetRelaxedFourCount());
    EXPECT_EQ(1, four_space_search.GetMaxRelaxedFourLength());
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
  
    EXPECT_EQ(2, four_space_search.GetRelaxedFourCount());
    EXPECT_EQ(1, four_space_search.GetMaxRelaxedFourLength());
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

    EXPECT_EQ(4, four_space_search.GetRelaxedFourCount());
    EXPECT_EQ(2, four_space_search.GetMaxRelaxedFourLength());
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

    EXPECT_EQ(4, four_space_search.GetRelaxedFourCount());
    EXPECT_EQ(2, four_space_search.GetMaxRelaxedFourLength());
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

    EXPECT_EQ(12, four_space_search.GetRelaxedFourCount());
    EXPECT_EQ(5, four_space_search.GetMaxRelaxedFourLength());
  }

  void UpdateMultiExpandableFourWhite()
  {
    //   A B C D E F G H I J K L M N O 
    // A x x ----------o ------------+ A 
    // B x . . . . . . . o . . . . . | B 
    // C | . . . . . . . . o . . . . | C 
    // D | . . * . . . . . . . * . . | D 
    // E | . . . . . . . . . . . . . | E 
    // F | . . . . . . . . . . . . . o F 
    // G | . . . . . x o o . o . x o | G 
    // H | . . . . . . x . . . . o . | H 
    // I | . . . . . . . . . . . . . | I 
    // J | . . . . . . . . . . . . . | J 
    // K | . . . . . . . . x . . . . | K 
    // L | . . * . . . . . . . * . . | L 
    // M | . . . . . . . . . . . . . | M 
    // N x . . . . . . . . . . . . . | N 
    // O x --------------------------x O 
    //   A B C D E F G H I J K L M N O 
    MoveList move_list("hhhgggigaokgoojcaahaabibbaofanmhjkngmg");
    BitBoard bit_board(move_list);

    FourSpaceSearch four_space_search(bit_board);
    four_space_search.ExpandFourSpace<kWhiteTurn>();

    const auto &reach_region = four_space_search.reach_region_;
    const auto &put_region = four_space_search.put_region_;

    const MovePosition expect_list[] = {
      kMoveKD, kMoveLE, kMoveJG, kMoveLG,
      kMoveLI, kMoveKJ, kMoveLC, kMoveLD,
      kMoveLF, kMoveLH, kMoveLJ, kMoveLK,
    };

    MoveBitSet expect_bit;

    for(const auto move : expect_list){
      expect_bit.set(move);
    }

    for(const auto move : GetAllInBoardMove()){
      EXPECT_EQ(expect_bit[move], !reach_region[move].empty());
      EXPECT_EQ(expect_bit[move], !put_region[move].empty());
    }

    EXPECT_EQ(12, four_space_search.GetRelaxedFourCount());
    EXPECT_EQ(5, four_space_search.GetMaxRelaxedFourLength());
  }

  void UpdateMultiExpandableFourBlack2()
  {
    //   A B C D E F G H I J K L M N O 
    // A x x x ------------x ----x x x A 
    // B x o o . . . . . . . . . o o x B 
    // C x o o . . . . . . . . . o o x C 
    // D | . . o . . . o . . . o . . | D 
    // E | . . . . . . . . . . . . . | E 
    // F | . . . . . . . . . . . . . x F 
    // G | . . . . . . . . . . . . . | G 
    // H | . . o . . . x . . . o . . | H 
    // I | . . . . . . . . . . . . . | I 
    // J | . . . . . . . . . . . . . | J 
    // K | . . . . . . . . . . . . . | K 
    // L | . . o . . . o . . . o . . | L 
    // M x o o . . . . . . . . . o o x M 
    // N x o o . . . . . . . . . o o x N 
    // O x x x --------x --------x x x O 
    //   A B C D E F G H I J K L M N O 
    MoveList move_list("hhbbaacbbabcabcccaddacbnaocnbobmancmcodlamnnoommnonmonmnmollomnboamcnancobmbmaldochdjalhofdhhohl");
    BitBoard bit_board(move_list);

    FourSpaceSearch four_space_search(bit_board);
    four_space_search.ExpandFourSpace<kBlackTurn>();

    const auto &reach_region = four_space_search.reach_region_;
    const auto &put_region = four_space_search.put_region_;

    const MovePosition expect_list[] = {
      kMoveDA, kMoveEA, kMoveAD, kMoveAE, 
      kMoveDO, kMoveEO, kMoveAK, kMoveAL, 
      kMoveKO, kMoveLO, kMoveOK, kMoveOL, 
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

    EXPECT_EQ(12, four_space_search.GetRelaxedFourCount());
    EXPECT_EQ(1, four_space_search.GetMaxRelaxedFourLength());
  }

  void UpdateMultiExpandableFourWhite2()
  {
    //   A B C D E F G H I J K L M N O 
    // A o o o ------------o ----o o o A 
    // B o x x . . . . . . . . . x x o B 
    // C o x x . . . . . . . . . x x o C 
    // D | . . x . . . x . . . x . . | D 
    // E | . . . . . . . . . . . . . | E 
    // F | . . . . . . . . . . . . . o F 
    // G | . . . . . . . . . . . . . | G 
    // H | . . x . . . x . . . x . . | H 
    // I | . . . . . . . . . . . . . | I 
    // J | . . . . . . . . . . . . . | J 
    // K | . . . . . . . . . . . . . | K 
    // L | . . x . . . x . . . x . . | L 
    // M o x x . . . . . . . . . x x o M 
    // N o x x . . . . . . . . . x x o N 
    // O o o o --------o --------o o o O 
    //   A B C D E F G H I J K L M N O 
    
    MoveList move_list("hhppbbaacbbabcabcccaddacbnaocnbobmancmcodlamnnoommnonmonmnmollomnboamcnancobmbmaldochdjalhofdhhohl");
    BitBoard bit_board(move_list);

    FourSpaceSearch four_space_search(bit_board);
    four_space_search.ExpandFourSpace<kWhiteTurn>();

    const auto &reach_region = four_space_search.reach_region_;
    const auto &put_region = four_space_search.put_region_;

    const MovePosition expect_list[] = {
      kMoveDA, kMoveEA, kMoveAD, kMoveAE, 
      kMoveDO, kMoveEO, kMoveAK, kMoveAL, 
      kMoveKO, kMoveLO, kMoveOK, kMoveOL, 
      kMoveKA, kMoveLA, kMoveOD, kMoveOE
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

    EXPECT_EQ(16, four_space_search.GetRelaxedFourCount());
    EXPECT_EQ(1, four_space_search.GetMaxRelaxedFourLength());
  }

  void UpdateMultiExpandableFourBlack3()
  {
    //   A B C D E F G H I J K L M N O 
    // A x x x ------------------x x x A 
    // B x o o . . . . . . . . . o o x B 
    // C x o o . . x . . . . . . o o x C 
    // D | . . o . . x . . . . o . . | D 
    // E | . . . . . . . . . . . . . | E 
    // F | . . . . . . . . . . . . . | F 
    // G | . . . . . . x . . . . . . | G 
    // H | . . o . . . x . . . o . . | H 
    // I | . . . . . . . . . . . . . | I 
    // J | . . . . . . . . . . . . . | J 
    // K | . . . . . . . . . . . . . | K 
    // L | . . o . . . o . . . o . . | L 
    // M x o o . . . . . . . . . o o x M 
    // N x o o . . . . . . . . . o o x N 
    // O x x x --------o --------x x x O 
    //   A B C D E F G H I J K L M N O 
    MoveList move_list("hhbbaacbbabcabcccaddacbnaocnbobmancmcodlamnnoommnonmonmnmollomnboamcnancobmbmaldochlfcdhgdlhhgho");
    BitBoard bit_board(move_list);

    FourSpaceSearch four_space_search(bit_board);
    four_space_search.ExpandFourSpace<kBlackTurn>();

    const auto &reach_region = four_space_search.reach_region_;
    const auto &put_region = four_space_search.put_region_;

    const MovePosition expect_list[] = {
      kMoveDA, kMoveEA, kMoveAD, kMoveAE, 
      kMoveKA, kMoveLA, kMoveOD, kMoveOE,
      kMoveAK, kMoveAL, kMoveDO, kMoveEO,
      kMoveKO, kMoveLO, kMoveOK, kMoveOL, 
      kMoveEB, kMoveHE, kMoveIF, kMoveJG, 
      kMoveHD, kMoveHF, kMoveHI, kMoveHJ, kMoveHK 
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

    EXPECT_EQ(25 + 1, four_space_search.GetRelaxedFourCount());  // 達四はダブルカウントされる
    EXPECT_EQ(4, four_space_search.GetMaxRelaxedFourLength());
  }

  void UpdateDiffcultTenYearFever()
  {
    // 十年フィーバー
    MoveList move_list("hhgigjfgjjfjeeifhkijlijhllefcjejhmdgmcdjmkjdbblkbefmblgmdnimibjcnbjmaemhaibgambnbocncahbcojndokbeankganljoabkaacmaadoafoochaoeonokoo");
    BitBoard bit_board(move_list);

    FourSpaceSearch four_space_search(bit_board);
    four_space_search.ExpandFourSpace<kBlackTurn>();

    cerr << "R-four count: " << four_space_search.GetRelaxedFourCount() << endl;
    cerr << "R-four length: " << four_space_search.GetMaxRelaxedFourLength() << endl;
    EXPECT_TRUE(four_space_search.GetMaxRelaxedFourLength() >= 73);
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

TEST_F(FourSpaceSearchTest, UpdateMultiExpandableFourWhiteTest)
{
  UpdateMultiExpandableFourWhite();
}

TEST_F(FourSpaceSearchTest, UpdateMultiExpandableFourBlack2Test)
{
  UpdateMultiExpandableFourBlack2();
}

TEST_F(FourSpaceSearchTest, UpdateMultiExpandableFourWhite2Test)
{
  UpdateMultiExpandableFourWhite2();
}

TEST_F(FourSpaceSearchTest, UpdateMultiExpandableFourBlack3Test)
{
  UpdateMultiExpandableFourBlack3();
}

TEST_F(FourSpaceSearchTest, UpdateDiffcultTenYearFeverTest)
{
  UpdateDiffcultTenYearFever();
}

}
