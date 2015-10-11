/* MoveTable.cpp 
 *
 * Kubo Ryosuke
 */

#include "MoveTable.h"
#include "core/board/Bitboard.h"

namespace sunfish {

namespace {

using namespace sunfish;

bool isFileEnabled(int file, Square baseSq, Direction dir) {
  if (dir == Direction::Left || dir == Direction::Right) {
    return true;
  } else if (dir == Direction::LeftUp || dir == Direction::RightDown) {
    int minFile = std::max(2, baseSq.getFile() + baseSq.getRank() - 8);
    int maxFile = std::min(8, baseSq.getFile() + baseSq.getRank() - 2);
    return file >= minFile && file <= maxFile;
  } else if (dir == Direction::LeftDown || dir == Direction::RightUp) {
    int minFile = std::max(2, baseSq.getFile() - baseSq.getRank() + 2);
    int maxFile = std::min(8, baseSq.getFile() - baseSq.getRank() + 8);
    return file >= minFile && file <= maxFile;
  } else {
    assert(false);
  }
}

int file2BitIndex(bool useBmi2, int file, Square baseSq, Direction dir) {
  int list[7] = { 2, 8, 7, 6, 5, 4, 3 };

  if (useBmi2) {
    int index = 0;
    for (int i = 0; i < 7; i++) {
      if (list[i] == file) {
        return index;
      } else if (isFileEnabled(list[i], baseSq, dir)) {
        index++;
      }
    }
  } else {
    int index = 6;
    for (int i = 6; i >= 0; i--) {
      if (list[i] == file) {
        return index;
      } else if (isFileEnabled(list[i], baseSq, dir)) {
        index--;
      }
    }
  }
  assert(false);
}

} // namespace

const DirectionMaskTable DirectionMaskTable::instance;
const DirectionMaskTable7x7 DirectionMaskTable7x7::instance;

#define GEN_MASK(table, dir, rdir, full) { \
SQUARE_EACH(from) { \
for (Square to = from; (full ? to : to.safety ## dir()).isValid(); to = to.safety ## dir()) { \
  if (full || to.safety ## rdir().isValid()) { \
    table[from.index()].set(to.index()); \
  } \
} \
} \
}

DirectionMaskTable::DirectionMaskTable() {
  SQUARE_EACH(sq) { up_[sq.index()].init(); }
  GEN_MASK(up_, Up, Down, true);

  SQUARE_EACH(sq) { down_[sq.index()].init(); }
  GEN_MASK(down_, Down, Up, true);

  SQUARE_EACH(sq) { left_[sq.index()].init(); }
  GEN_MASK(left_, Left, Right, true);

  SQUARE_EACH(sq) { right_[sq.index()].init(); }
  GEN_MASK(right_, Right, Left, true);

  SQUARE_EACH(sq) { leftUp_[sq.index()].init(); }
  GEN_MASK(leftUp_, LeftUp, RightDown, true);

  SQUARE_EACH(sq) { rightDown_[sq.index()].init(); }
  GEN_MASK(rightDown_, RightDown, LeftUp, true);

  SQUARE_EACH(sq) { rightUp_[sq.index()].init(); }
  GEN_MASK(rightUp_, RightUp, LeftDown, true);

  SQUARE_EACH(sq) { leftDown_[sq.index()].init(); }
  GEN_MASK(leftDown_, LeftDown, RightUp, true);
}

DirectionMaskTable7x7::DirectionMaskTable7x7() {
  SQUARE_EACH(sq) { rank_[sq.index()].init(); }
  GEN_MASK(rank_, Left, Right, false);
  GEN_MASK(rank_, Right, Left, false);

  SQUARE_EACH(sq) { leftUpX_[sq.index()].init(); }
  GEN_MASK(leftUpX_, LeftUp, RightDown, false);
  GEN_MASK(leftUpX_, RightDown, LeftUp, false);

  SQUARE_EACH(sq) { rightUpX_[sq.index()].init(); }
  GEN_MASK(rightUpX_, RightUp, LeftDown, false);
  GEN_MASK(rightUpX_, LeftDown, RightUp, false);
}

#undef GEN_MASK

#if !USE_BMI2
const MagicNumberTable MagicNumberTable::instance;

MagicNumberTable::MagicNumberTable() {
  auto set = [](Square sq, Square baseSq, uint64_t& magicLow, uint64_t& magicHigh, Direction dir) {
    int index = file2BitIndex(false, sq.getFile(), baseSq, dir);
    if (Bitboard::isLow(sq)) {
      magicLow |= 1ULL << (64 - 7 + index - sq.index());
    } else {
      magicHigh |= 1ULL << (64 - 7 + index - (sq.index() - Bitboard::LowBits));
    }
  };

  SQUARE_EACH(baseSq) {
    {
      uint64_t magicLow = 0ULL;
      uint64_t magicHigh = 0ULL;
      for (Square sq = baseSq.safetyLeftUp(); sq.safetyLeftUp().isValid(); sq = sq.safetyLeftUp()) {
        set(sq, baseSq, magicLow, magicHigh, Direction::LeftUp);
      }
      for (Square sq = baseSq.safetyRightDown(); sq.safetyRightDown().isValid(); sq = sq.safetyRightDown()) {
        set(sq, baseSq, magicLow, magicHigh, Direction::LeftUp);
      }
      if (baseSq.safetyLeftUp().isValid() && baseSq.safetyRightDown().isValid()) {
        set(baseSq, baseSq, magicLow, magicHigh, Direction::LeftUp);
      }
      leftUp_[baseSq] = Bitboard(magicHigh, magicLow);
    }
  }

  SQUARE_EACH(baseSq) {
    {
      uint64_t magicLow = 0ULL;
      uint64_t magicHigh = 0ULL;
      for (Square sq = baseSq.safetyRightUp(); sq.safetyRightUp().isValid(); sq = sq.safetyRightUp()) {
        set(sq, baseSq, magicLow, magicHigh, Direction::RightUp);
      }
      for (Square sq = baseSq.safetyLeftDown(); sq.safetyLeftDown().isValid(); sq = sq.safetyLeftDown()) {
        set(sq, baseSq, magicLow, magicHigh, Direction::RightUp);
      }
      if (baseSq.safetyRightUp().isValid() && baseSq.safetyLeftDown().isValid()) {
        set(baseSq, baseSq, magicLow, magicHigh, Direction::RightUp);
      }

      rightUp_[baseSq] = Bitboard(magicHigh, magicLow);
    }
  }

  Bitboard rank_[Square::RankN];
  for (int rank = 1; rank <= 9; rank++) {
    uint64_t magicLow = 0ULL;
    uint64_t magicHigh = 0ULL;
    for (int file = 2; file <= 8; file++) {
      Square sq(file, rank);
      set(sq, sq, magicLow, magicHigh, Direction::Right);
    }
    rank_[rank-1] = Bitboard(magicHigh, magicLow);
  }
  SQUARE_EACH(sq) {
    rank_[sq] = Bitboard(rank_[sq.getRank()-1].high(), rank_[sq.getRank()-1].low());
  }
}

#endif // !USE_BMI2

const MovePatternTable MovePatternTable::instance;

MovePatternTable::MovePatternTable() {
#if USE_BMI2
  bool useBmi2 = true;
#else
  bool useBmi2 = false;
#endif // USE_BMI2

  SQUARE_EACH(sq) {
    for (unsigned i = 0; i < 0x80; i++) {
      up_[sq.index()][i].init();
      down_[sq.index()][i].init();
      file_[sq.index()][i].init();
      rank_[sq.index()][i].init();
      leftUpX_[sq.index()][i].init();
      rightUpX_[sq.index()][i].init();
      leftUp_[sq.index()][i].init();
      leftDown_[sq.index()][i].init();
      rightUp_[sq.index()][i].init();
      rightDown_[sq.index()][i].init();
      left_[sq.index()][i].init();
      right_[sq.index()][i].init();
    }
  }

  SQUARE_EACH(baseSq) {
    for (unsigned b = 0; b < 0x80; b++) {
      // up
      for (Square sq = baseSq.safetyUp(); sq.isValid() && sq.getRank() >= 1; sq = sq.safetyUp()) {
        up_[baseSq.index()][b].set(sq);
        file_[baseSq.index()][b].set(sq);
        if (b & (1 << (sq.getRank() - 2))) { break; }
      }
      // down
      for (Square sq = baseSq.safetyDown(); sq.isValid() && sq.getRank() <= 9; sq = sq.safetyDown()) {
        down_[baseSq.index()][b].set(sq);
        file_[baseSq.index()][b].set(sq);
        if (b & (1 << (sq.getRank() - 2))) { break; }
      }
      // left
      for (Square sq = baseSq.safetyLeft(); sq.isValid(); sq = sq.safetyLeft()) {
        rank_[baseSq.index()][b].set(sq);
        left_[baseSq.index()][b].set(sq);
        if (sq.getFile() == 9) { break; }
        if (b & (1 << file2BitIndex(useBmi2, sq.getFile(), baseSq, Direction::Left))) { break; }
      }
      // right
      for (Square sq = baseSq.safetyRight(); sq.isValid(); sq = sq.safetyRight()) {
        rank_[baseSq.index()][b].set(sq);
        right_[baseSq.index()][b].set(sq);
        if (sq.getFile() == 1) { break; }
        if (b & (1 << file2BitIndex(useBmi2, sq.getFile(), baseSq, Direction::Right))) { break; }
      }
      // left-up
      for (Square sq = baseSq.safetyLeftUp(); sq.isValid(); sq = sq.safetyLeftUp()) {
        leftUpX_[baseSq.index()][b].set(sq);
        leftUp_[baseSq.index()][b].set(sq);
        if (sq.getFile() == 9) { break; }
        if (b & (1 << file2BitIndex(useBmi2, sq.getFile(), baseSq, Direction::LeftUp))) { break; }
      }
      // right-down
      for (Square sq = baseSq.safetyRightDown(); sq.isValid(); sq = sq.safetyRightDown()) {
        leftUpX_[baseSq.index()][b].set(sq);
        rightDown_[baseSq.index()][b].set(sq);
        if (sq.getFile() == 1) { break; }
        if (b & (1 << file2BitIndex(useBmi2, sq.getFile(), baseSq, Direction::RightDown))) { break; }
      }
      // right-up
      for (Square sq = baseSq.safetyRightUp(); sq.isValid(); sq = sq.safetyRightUp()) {
        rightUpX_[baseSq.index()][b].set(sq);
        rightUp_[baseSq.index()][b].set(sq);
        if (sq.getFile() == 1) { break; }
        if (b & (1 << file2BitIndex(useBmi2, sq.getFile(), baseSq, Direction::RightUp))) { break; }
      }
      // left-down
      for (Square sq = baseSq.safetyLeftDown(); sq.isValid(); sq = sq.safetyLeftDown()) {
        rightUpX_[baseSq.index()][b].set(sq);
        leftDown_[baseSq.index()][b].set(sq);
        if (sq.getFile() == 9) { break; }
        if (b & (1 << file2BitIndex(useBmi2, sq.getFile(), baseSq, Direction::LeftDown))) { break; }
      }
    }
  }
}

namespace {

void generateOneStepMoveTable(Bitboard* table, Piece type) {
  SQUARE_EACH(sq) {
    Bitboard bb;
    bb.init();
    switch (type.index()) {
    case Piece::BPawn:
      bb |= Bitboard(sq.safetyUp());
      break;
    case Piece::BKnight:
      bb |= Bitboard(sq.safetyUp(2).safetyLeft());
      bb |= Bitboard(sq.safetyUp(2).safetyRight());
      break;
    case Piece::BSilver:
      bb |= Bitboard(sq.safetyUp().safetyLeft());
      bb |= Bitboard(sq.safetyUp());
      bb |= Bitboard(sq.safetyUp().safetyRight());
      bb |= Bitboard(sq.safetyDown().safetyLeft());
      bb |= Bitboard(sq.safetyDown().safetyRight());
      break;
    case Piece::BGold:
      bb |= Bitboard(sq.safetyUp().safetyLeft());
      bb |= Bitboard(sq.safetyUp());
      bb |= Bitboard(sq.safetyUp().safetyRight());
      bb |= Bitboard(sq.safetyLeft());
      bb |= Bitboard(sq.safetyRight());
      bb |= Bitboard(sq.safetyDown());
      break;
    case Piece::WPawn:
      bb |= Bitboard(sq.safetyDown());
      break;
    case Piece::WKnight:
      bb |= Bitboard(sq.safetyDown(2).safetyLeft());
      bb |= Bitboard(sq.safetyDown(2).safetyRight());
      break;
    case Piece::WSilver:
      bb |= Bitboard(sq.safetyDown().safetyLeft());
      bb |= Bitboard(sq.safetyDown());
      bb |= Bitboard(sq.safetyDown().safetyRight());
      bb |= Bitboard(sq.safetyUp().safetyLeft());
      bb |= Bitboard(sq.safetyUp().safetyRight());
      break;
    case Piece::WGold:
      bb |= Bitboard(sq.safetyDown().safetyLeft());
      bb |= Bitboard(sq.safetyDown());
      bb |= Bitboard(sq.safetyDown().safetyRight());
      bb |= Bitboard(sq.safetyLeft());
      bb |= Bitboard(sq.safetyRight());
      bb |= Bitboard(sq.safetyUp());
      break;
    case Piece::Bishop:
    case Piece::Dragon:
      bb |= Bitboard(sq.safetyUp().safetyLeft());
      bb |= Bitboard(sq.safetyUp().safetyRight());
      bb |= Bitboard(sq.safetyDown().safetyLeft());
      bb |= Bitboard(sq.safetyDown().safetyRight());
      break;
    case Piece::Rook:
    case Piece::Horse:
      bb |= Bitboard(sq.safetyUp());
      bb |= Bitboard(sq.safetyLeft());
      bb |= Bitboard(sq.safetyRight());
      bb |= Bitboard(sq.safetyDown());
      break;
    case Piece::King:
      bb |= Bitboard(sq.safetyUp().safetyLeft());
      bb |= Bitboard(sq.safetyUp());
      bb |= Bitboard(sq.safetyUp().safetyRight());
      bb |= Bitboard(sq.safetyLeft());
      bb |= Bitboard(sq.safetyRight());
      bb |= Bitboard(sq.safetyDown().safetyLeft());
      bb |= Bitboard(sq.safetyDown());
      bb |= Bitboard(sq.safetyDown().safetyRight());
      break;
    default:
      assert(false);
    }
    table[sq.index()] = Bitboard(bb.high(), bb.low());
  }
}

}

const MoveTables MoveTables::instance;

MoveTables::MoveTables() {
 generateOneStepMoveTable(bpawn_, Piece::BPawn);
 generateOneStepMoveTable(bknight_, Piece::BKnight);
 generateOneStepMoveTable(bsilver_, Piece::BSilver);
 generateOneStepMoveTable(bgold_, Piece::BGold);
 generateOneStepMoveTable(wpawn_, Piece::WPawn);
 generateOneStepMoveTable(wknight_, Piece::WKnight);
 generateOneStepMoveTable(wsilver_, Piece::WSilver);
 generateOneStepMoveTable(wgold_, Piece::WGold);
 generateOneStepMoveTable(bishop1_, Piece::Bishop);
 generateOneStepMoveTable(rook1_, Piece::Rook);
 generateOneStepMoveTable(king_, Piece::King);
 generateOneStepMoveTable(horseOneStepMove_, Piece::Horse);
 generateOneStepMoveTable(dragonOneStepMove_, Piece::Dragon);
}

} // namespace sunfish
