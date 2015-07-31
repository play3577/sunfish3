/* Evaluator.h
 *
 * Kubo Ryosuke
 */

#ifndef SUNFISH_EVALUATOR__
#define SUNFISH_EVALUATOR__

#include "EvaluateTable.h"
#include "Value.h"
#include "core/board/Board.h"
#include "Material.h"
#include <memory>

namespace sunfish {

class ValuePair {
public:

  static CONSTEXPR_CONST int PositionalScale = 32;

private:

  Value material_;
  Value positional_;

public:

  ValuePair() : material_(0), positional_(0) {}
  ValuePair(const Value& material, const Value& positional) : material_(material), positional_(positional) {}

  Value material() const {
    return material_;
  }

  Value positional() const {
    return positional_;
  }

  Value value() const {
    return material_ + positional_ / PositionalScale;
  }

  ValuePair operator+(const ValuePair& right) const {
    return ValuePair(material_ + right.material_, positional_ + right.positional_);
  }

};

enum {
  KPP_HBPAWN   = 0,
  KPP_HWPAWN   = KPP_HBPAWN   + 19,
  KPP_HBLANCE  = KPP_HWPAWN   + 19,
  KPP_HWLANCE  = KPP_HBLANCE  + 5,
  KPP_HBKNIGHT = KPP_HWLANCE  + 5,
  KPP_HWKNIGHT = KPP_HBKNIGHT + 5,
  KPP_HBSILVER = KPP_HWKNIGHT + 5,
  KPP_HWSILVER = KPP_HBSILVER + 5,
  KPP_HBGOLD   = KPP_HWSILVER + 5,
  KPP_HWGOLD   = KPP_HBGOLD   + 5,
  KPP_HBBISHOP = KPP_HWGOLD   + 5,
  KPP_HWBISHOP = KPP_HBBISHOP + 3,
  KPP_HBROOK   = KPP_HWBISHOP + 3,
  KPP_HWROOK   = KPP_HBROOK   + 3,
  KPP_BBPAWN   = KPP_HWROOK   + 3,
  KPP_BWPAWN   = KPP_BBPAWN   + 81 - 9,
  KPP_BBLANCE  = KPP_BWPAWN   + 81 - 9,
  KPP_BWLANCE  = KPP_BBLANCE  + 81 - 9,
  KPP_BBKNIGHT = KPP_BWLANCE  + 81 - 9,
  KPP_BWKNIGHT = KPP_BBKNIGHT + 81 - 18,
  KPP_BBSILVER = KPP_BWKNIGHT + 81 - 18,
  KPP_BWSILVER = KPP_BBSILVER + 81,
  KPP_BBGOLD   = KPP_BWSILVER + 81,
  KPP_BWGOLD   = KPP_BBGOLD   + 81,
  KPP_BBBISHOP = KPP_BWGOLD   + 81,
  KPP_BWBISHOP = KPP_BBBISHOP + 81,
  KPP_BBHORSE  = KPP_BWBISHOP + 81,
  KPP_BWHORSE  = KPP_BBHORSE  + 81,
  KPP_BBROOK   = KPP_BWHORSE  + 81,
  KPP_BWROOK   = KPP_BBROOK   + 81,
  KPP_BBDRAGON = KPP_BWROOK   + 81,
  KPP_BWDRAGON = KPP_BBDRAGON + 81,
  KPP_MAX      = KPP_BWDRAGON + 81,
  KPP_SIZE     = KPP_MAX*(KPP_MAX+1)/2,
  KPP_ALL      = 81 * KPP_SIZE,

  KKP_HPAWN   = 0,
  KKP_HLANCE  = KKP_HPAWN   + 19,
  KKP_HKNIGHT = KKP_HLANCE  + 5,
  KKP_HSILVER = KKP_HKNIGHT + 5,
  KKP_HGOLD   = KKP_HSILVER + 5,
  KKP_HBISHOP = KKP_HGOLD   + 5,
  KKP_HROOK   = KKP_HBISHOP + 3,
  KKP_BPAWN   = KKP_HROOK + 3,
  KKP_BLANCE  = KKP_BPAWN   + 81 - 9,
  KKP_BKNIGHT = KKP_BLANCE  + 81 - 9,
  KKP_BSILVER = KKP_BKNIGHT + 81 - 18,
  KKP_BGOLD   = KKP_BSILVER + 81,
  KKP_BBISHOP = KKP_BGOLD   + 81,
  KKP_BHORSE  = KKP_BBISHOP + 81,
  KKP_BROOK   = KKP_BHORSE  + 81,
  KKP_BDRAGON = KKP_BROOK   + 81,
  KKP_MAX     = KKP_BDRAGON + 81,
  KKP_ALL     = 81 * 81 * KKP_MAX,
};

template <class T>
class Feature {
public:

  using ValueType = T;

  struct Table {
    ValueType kpp[81][KPP_SIZE];
    ValueType kkp[81][81][KKP_MAX];
  };

  Table* t_;

  bool allocated;

protected:

  Feature() : t_(nullptr) {
    t_ = new Table();
    allocated = true;
    assert(t_ != nullptr);
  }

  Feature(Feature& ref) : t_(nullptr) {
    t_ = ref.t_;
    allocated = false;
    assert(t_ != nullptr);
  }

  Feature(const Feature&) = delete;
  Feature(Feature&&) = delete;

  ~Feature() {
    if (allocated && t_ != nullptr) {
      delete t_;
      t_ = nullptr;
    }
  }

public:

  /**
   * ファイルからパラメータを読み込みます。
   * @param filename
   */
  bool readFile(const char* filename);

  /**
   * ファイルからパラメータを読み込みます。
   * @param filename
   */
  bool readFile(const std::string& filename) {
    return readFile(filename.c_str());
  }

  /**
   * ファイルにパラメータを書き出します。
   * @param filename
   */
  bool writeFile(const char* filename) const;

  /**
   * ファイルにパラメータを書き出します。
   * @param filename
   */
  bool writeFile(const std::string& filename) const {
    return writeFile(filename.c_str());
  }

  template <class U, bool update>
  U extract(const Board& board, U delta);

};

class Evaluator : public Feature<int16_t> {
public:

  enum class InitType : int {
    File,
    Zero,
    Random,
  };

private:

  EvaluateTable<18> evaluateCache_;

  EvaluateTable<18> estimateCache_;

  std::shared_ptr<Table> readFvBin();

  void convertFromFvBin(Table* fvbin);

  /**
   * 局面の駒割りを算出します。
   * @param board
   */
  Value evaluateMaterial_(const Board& board) const;

  /**
   * 局面の駒割りを除いた評価値を算出します。
   * @param board
   */
  Value evaluate_(const Board& board);

  /**
   * 指定した指し手による評価値の変化値を算出します。
   * @param board 着手後の局面を指定します。
   * @param prevValuePair
   * @param move
   */
  template <bool black>
  ValuePair evaluateDiff_(const Board& board, const ValuePair& prevValuePair, const Move& move);

  template <bool black, bool isKing, bool positionalOnly>
  Value estimate_(const Board& board, const Move& move);

public:

  Evaluator(InitType initType = InitType::File);

  Evaluator(Evaluator& ref);

  void init();
  void initRandom();

  /**
   * ファイルからパラメータを読み込みます。
   */
  bool readFile();

  /**
   * ファイルからパラメータを読み込みます。
   * @param filename
   */
  bool readFile(const char* filename) {
    return Feature<int16_t>::readFile(filename);
  }

  /**
   * ファイルからパラメータを読み込みます。
   * @param filename
   */
  bool readFile(const std::string& filename) {
    return Feature<int16_t>::readFile(filename);
  }

  /**
   * ファイルにパラメータを書き出します。
   */
  bool writeFile() const;

  /**
   * ファイルにパラメータを書き出します。
   * @param filename
   */
  bool writeFile(const char* filename) {
    return Feature<int16_t>::writeFile(filename);
  }

  /**
   * ファイルにパラメータを書き出します。
   * @param filename
   */
  bool writeFile(const std::string& filename) const {
    return Feature<int16_t>::writeFile(filename);
  }

  /**
   * fv.bin があれば読み込んで並べ替えを行います。
   */
  bool convertFromFvBin();

  /**
   * 局面の評価値を算出します。
   * @param board
   */
  ValuePair evaluate(const Board& board) {
    return ValuePair(evaluateMaterial_(board), evaluate_(board));
  }

  /**
   * 指定した指し手に基づき評価値の差分計算を行います。
   * @param board 着手後の局面を指定します。
   * @param prevValuePair
   * @param move
   */
  ValuePair evaluateDiff(const Board& board, const ValuePair& prevValuePair, const Move& move) {
    if (!board.isBlack()) {
      return evaluateDiff_<true>(board, prevValuePair, move);
    } else {
      return evaluateDiff_<false>(board, prevValuePair, move);
    }
  }

  template <bool positionalOnly = false>
  Value estimate(const Board& board, const Move& move) {
    if (board.isBlack()) {
      return (move.piece() == Piece::King ?
              estimate_<true, true, positionalOnly>(board, move):
              estimate_<true, false, positionalOnly>(board, move));
    } else {
      return (move.piece() == Piece::King ?
              estimate_<false, true, positionalOnly>(board, move):
              estimate_<false, false, positionalOnly>(board, move));
    }
  }

  const Table& table() const {
    return *t_;
  }

  void clearCache() {
    evaluateCache_.init();
    estimateCache_.init();
  }

};

} // namespace sunfish

#endif // SUNFISH_EVALUATOR__
