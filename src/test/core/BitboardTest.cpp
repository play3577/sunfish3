/* BitboardTest.cpp
 *
 * Kubo Ryosuke
 */

#if !defined(NDEBUG)

#include "test/Test.h"
#include "core/board/Bitboard.h"

using namespace sunfish;

TEST(BitboardTest, test) {
  {
    Bitboard bb;
    bb.init();
    ASSERT_EQ(Bitboard(0x00ll, 0x00ll), bb);
  }

  {
    Bitboard bb;
    bb.init();
    bb.set(S55);
    ASSERT_EQ(Bitboard(S55), bb);
    ASSERT(bb.check(S55));
  }

  {
    Bitboard bb;
    bb.init();
    bb.set(S91);
    ASSERT_EQ(Bitboard(S91), bb);
    ASSERT(bb.check(S91));
  }

  {
    Bitboard bb;
    bb.init();
    bb.set(S19);
    ASSERT_EQ(Bitboard(S19), bb);
    ASSERT(bb.check(S19));
  }

  {
    Bitboard bb;
    bb.init();
    bb.set(S38);
    ASSERT_EQ(Bitboard(S38), bb);
    ASSERT(bb.check(S38));
  }

  {
    Bitboard bb;
    bb.init();
    bb.set(S84);
    ASSERT_EQ(Bitboard(S84), bb);
    ASSERT(bb.check(S84));
  }

  {
    ASSERT_EQ(Bitboard::file(1).toString2D(),
      "000000001\n"
      "000000001\n"
      "000000001\n"
      "000000001\n"
      "000000001\n"
      "000000001\n"
      "000000001\n"
      "000000001\n"
      "000000001\n");
    ASSERT_EQ(Bitboard::file(2).toString2D(),
      "000000010\n"
      "000000010\n"
      "000000010\n"
      "000000010\n"
      "000000010\n"
      "000000010\n"
      "000000010\n"
      "000000010\n"
      "000000010\n");
    ASSERT_EQ(Bitboard::file(3).toString2D(),
      "000000100\n"
      "000000100\n"
      "000000100\n"
      "000000100\n"
      "000000100\n"
      "000000100\n"
      "000000100\n"
      "000000100\n"
      "000000100\n");
    ASSERT_EQ(Bitboard::file(4).toString2D(),
      "000001000\n"
      "000001000\n"
      "000001000\n"
      "000001000\n"
      "000001000\n"
      "000001000\n"
      "000001000\n"
      "000001000\n"
      "000001000\n");
    ASSERT_EQ(Bitboard::file(5).toString2D(),
      "000010000\n"
      "000010000\n"
      "000010000\n"
      "000010000\n"
      "000010000\n"
      "000010000\n"
      "000010000\n"
      "000010000\n"
      "000010000\n");
    ASSERT_EQ(Bitboard::file(6).toString2D(),
      "000100000\n"
      "000100000\n"
      "000100000\n"
      "000100000\n"
      "000100000\n"
      "000100000\n"
      "000100000\n"
      "000100000\n"
      "000100000\n");
    ASSERT_EQ(Bitboard::file(7).toString2D(),
      "001000000\n"
      "001000000\n"
      "001000000\n"
      "001000000\n"
      "001000000\n"
      "001000000\n"
      "001000000\n"
      "001000000\n"
      "001000000\n");
    ASSERT_EQ(Bitboard::file(8).toString2D(),
      "010000000\n"
      "010000000\n"
      "010000000\n"
      "010000000\n"
      "010000000\n"
      "010000000\n"
      "010000000\n"
      "010000000\n"
      "010000000\n");
    ASSERT_EQ(Bitboard::file(9).toString2D(),
      "100000000\n"
      "100000000\n"
      "100000000\n"
      "100000000\n"
      "100000000\n"
      "100000000\n"
      "100000000\n"
      "100000000\n"
      "100000000\n");
    ASSERT_EQ(Bitboard::notFile(1).toString2D(),
      "111111110\n"
      "111111110\n"
      "111111110\n"
      "111111110\n"
      "111111110\n"
      "111111110\n"
      "111111110\n"
      "111111110\n"
      "111111110\n");
    ASSERT_EQ(Bitboard::notFile(2).toString2D(),
      "111111101\n"
      "111111101\n"
      "111111101\n"
      "111111101\n"
      "111111101\n"
      "111111101\n"
      "111111101\n"
      "111111101\n"
      "111111101\n");
    ASSERT_EQ(Bitboard::notFile(3).toString2D(),
      "111111011\n"
      "111111011\n"
      "111111011\n"
      "111111011\n"
      "111111011\n"
      "111111011\n"
      "111111011\n"
      "111111011\n"
      "111111011\n");
    ASSERT_EQ(Bitboard::notFile(4).toString2D(),
      "111110111\n"
      "111110111\n"
      "111110111\n"
      "111110111\n"
      "111110111\n"
      "111110111\n"
      "111110111\n"
      "111110111\n"
      "111110111\n");
    ASSERT_EQ(Bitboard::notFile(5).toString2D(),
      "111101111\n"
      "111101111\n"
      "111101111\n"
      "111101111\n"
      "111101111\n"
      "111101111\n"
      "111101111\n"
      "111101111\n"
      "111101111\n");
    ASSERT_EQ(Bitboard::notFile(6).toString2D(),
      "111011111\n"
      "111011111\n"
      "111011111\n"
      "111011111\n"
      "111011111\n"
      "111011111\n"
      "111011111\n"
      "111011111\n"
      "111011111\n");
    ASSERT_EQ(Bitboard::notFile(7).toString2D(),
      "110111111\n"
      "110111111\n"
      "110111111\n"
      "110111111\n"
      "110111111\n"
      "110111111\n"
      "110111111\n"
      "110111111\n"
      "110111111\n");
    ASSERT_EQ(Bitboard::notFile(8).toString2D(),
      "101111111\n"
      "101111111\n"
      "101111111\n"
      "101111111\n"
      "101111111\n"
      "101111111\n"
      "101111111\n"
      "101111111\n"
      "101111111\n");
    ASSERT_EQ(Bitboard::notFile(9).toString2D(),
      "011111111\n"
      "011111111\n"
      "011111111\n"
      "011111111\n"
      "011111111\n"
      "011111111\n"
      "011111111\n"
      "011111111\n"
      "011111111\n");
  }

  {
    ASSERT_EQ(BPawnMovable.toString2D(),
      "000000000\n"
      "111111111\n"
      "111111111\n"
      "111111111\n"
      "111111111\n"
      "111111111\n"
      "111111111\n"
      "111111111\n"
      "111111111\n");
    ASSERT_EQ(BLanceMovable.toString2D(),
      "000000000\n"
      "111111111\n"
      "111111111\n"
      "111111111\n"
      "111111111\n"
      "111111111\n"
      "111111111\n"
      "111111111\n"
      "111111111\n");
    ASSERT_EQ(BKnightMovable.toString2D(),
      "000000000\n"
      "000000000\n"
      "111111111\n"
      "111111111\n"
      "111111111\n"
      "111111111\n"
      "111111111\n"
      "111111111\n"
      "111111111\n");
    ASSERT_EQ(WPawnMovable.toString2D(),
      "111111111\n"
      "111111111\n"
      "111111111\n"
      "111111111\n"
      "111111111\n"
      "111111111\n"
      "111111111\n"
      "111111111\n"
      "000000000\n");
    ASSERT_EQ(WLanceMovable.toString2D(),
      "111111111\n"
      "111111111\n"
      "111111111\n"
      "111111111\n"
      "111111111\n"
      "111111111\n"
      "111111111\n"
      "111111111\n"
      "000000000\n");
    ASSERT_EQ(WKnightMovable.toString2D(),
      "111111111\n"
      "111111111\n"
      "111111111\n"
      "111111111\n"
      "111111111\n"
      "111111111\n"
      "111111111\n"
      "000000000\n"
      "000000000\n");
    ASSERT_EQ(BPromotable.toString2D(),
      "111111111\n"
      "111111111\n"
      "111111111\n"
      "000000000\n"
      "000000000\n"
      "000000000\n"
      "000000000\n"
      "000000000\n"
      "000000000\n");
    ASSERT_EQ(WPromotable.toString2D(),
      "000000000\n"
      "000000000\n"
      "000000000\n"
      "000000000\n"
      "000000000\n"
      "000000000\n"
      "111111111\n"
      "111111111\n"
      "111111111\n");
    ASSERT_EQ(BPromotable2.toString2D(),
      "111111111\n"
      "111111111\n"
      "000000000\n"
      "000000000\n"
      "000000000\n"
      "000000000\n"
      "000000000\n"
      "000000000\n"
      "000000000\n");
    ASSERT_EQ(WPromotable2.toString2D(),
      "000000000\n"
      "000000000\n"
      "000000000\n"
      "000000000\n"
      "000000000\n"
      "000000000\n"
      "000000000\n"
      "111111111\n"
      "111111111\n");
  }
}

TEST(BitboardTest, testShift) {
  ASSERT_EQ(Bitboard(S54), Bitboard(S55).up());
  ASSERT_EQ(Bitboard(S56), Bitboard(S55).down());

  ASSERT_EQ(Bitboard(S53), Bitboard(S55).up(2));
  ASSERT_EQ(Bitboard(S57), Bitboard(S55).down(2));
}

TEST(BitboardTest, testCount) {
  {
    Bitboard bb;
    bb.init();
    ASSERT_EQ(0, bb.count());
  }

  {
    Bitboard bb;
    bb.init();
    bb.set(S33);
    bb.set(S48);
    bb.set(S82);
    bb.set(S99);
    ASSERT_EQ(4, bb.count());
  }

  {
    Bitboard bb;
    bb.init();
    bb.set(S15);
    bb.set(S24);
    bb.set(S28);
    bb.set(S49);
    bb.set(S57);
    bb.set(S61);
    bb.set(S69);
    bb.set(S74);
    bb.set(S85);
    bb.set(S94);
    bb.set(S97);
    ASSERT_EQ(11, bb.count());
  }
}

TEST(BitboardTest, testIterate) {
  Bitboard bb;
  Square sq;
  bb.init();
  bb.set(S95);
  bb.set(S62);
  bb.set(S55);
  bb.set(S41);
  bb.set(S48);
  bb.set(S27);

  ASSERT_EQ(S95, bb.getFirst());
  ASSERT_EQ(S27, bb.getLast());
  sq = bb.pickFirst();
  ASSERT_EQ(S95, sq.index());

  ASSERT_EQ(S62, bb.getFirst());
  ASSERT_EQ(S27, bb.getLast());
  sq = bb.pickFirst();
  ASSERT_EQ(S62, sq.index());

  ASSERT_EQ(S55, bb.getFirst());
  ASSERT_EQ(S27, bb.getLast());
  sq = bb.pickFirst();
  ASSERT_EQ(S55, sq.index());

  ASSERT_EQ(S41, bb.getFirst());
  ASSERT_EQ(S27, bb.getLast());
  sq = bb.pickFirst();
  ASSERT_EQ(S41, sq.index());

  ASSERT_EQ(S48, bb.getFirst());
  ASSERT_EQ(S27, bb.getLast());
  sq = bb.pickFirst();
  ASSERT_EQ(S48, sq.index());

  ASSERT_EQ(S27, bb.getFirst());
  ASSERT_EQ(S27, bb.getLast());
  sq = bb.pickFirst();
  ASSERT_EQ(S27, sq.index());

  ASSERT_EQ(Square::Invalid, bb.getFirst());
  ASSERT_EQ(Square::Invalid, bb.getLast());
  sq = bb.pickFirst();
  ASSERT(sq.isInvalid());
}

#endif // !defined(NDEBUG)
