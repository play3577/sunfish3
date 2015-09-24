/* EvaluatorTest.cpp
 *
 * Kubo Ryosuke
 */

#if !defined(NDEBUG)

#include "test/Test.h"
#include "searcher/eval/Evaluator.h"
#include "core/record/CsaReader.h"

using namespace sunfish;

TEST(EvaluatorTest, testEvaluateDiff) {

  Evaluator eval(Evaluator::InitType::Random);

  {
    // 盤上の駒を動かす先手の手
    std::string src =
"P1-KY-KE-GI-KI-OU-KI-GI-KE-KY\n"
"P2 * -HI *  *  *  *  * -KA * \n"
"P3-FU * -FU-FU-FU-FU-FU-FU-FU\n"
"P4 * -FU *  *  *  *  *  *  * \n"
"P5 *  *  *  *  *  *  *  *  * \n"
"P6 *  * +FU *  *  *  *  *  * \n"
"P7+FU+FU * +FU+FU+FU+FU+FU+FU\n"
"P8 * +KA *  *  *  *  * +HI * \n"
"P9+KY+KE+GI+KI+OU+KI+GI+KE+KY\n"
"P+\n"
"P-\n"
"+\n";
    std::istringstream iss(src);
    Board board;
    CsaReader::readBoard(iss, board);
    Move move(Piece::Pawn, S27, S26, false);

    auto prevValuePair = eval.evaluate(board);
    board.makeMove(move);
    auto valuePair = eval.evaluateDiff(board, prevValuePair, move);
    auto correctValuePair = eval.evaluate(board);

    ASSERT_EQ(correctValuePair.material().int32(), valuePair.material().int32());
    ASSERT_EQ(correctValuePair.positional().int32(), valuePair.positional().int32());
  }

  {
    // 先手が駒を打つ手
    std::string src =
"P1-KY-KE-GI-KI-OU * -GI-KE-KY\n"
"P2 *  *  *  *  *  * -KI-KA * \n"
"P3-FU * -FU-FU-FU-FU-FU-FU-FU\n"
"P4 *  *  *  *  *  *  *  *  * \n"
"P5 *  *  *  *  *  *  *  *  * \n"
"P6 * -HI *  *  *  *  *  *  * \n"
"P7+FU * +FU+FU+FU+FU+FU * +FU\n"
"P8 * +KA+KI *  *  *  * +HI * \n"
"P9+KY+KE+GI * +OU+KI+GI+KE+KY\n"
"P+00FU00FU\n"
"P-00FU\n"
"+\n";
    std::istringstream iss(src);
    Board board;
    CsaReader::readBoard(iss, board);
    Move move(Piece::Pawn, S87);

    auto prevValuePair = eval.evaluate(board);
    board.makeMove(move);
    auto valuePair = eval.evaluateDiff(board, prevValuePair, move);
    auto correctValuePair = eval.evaluate(board);

    ASSERT_EQ(correctValuePair.material().int32(), valuePair.material().int32());
    ASSERT_EQ(correctValuePair.positional().int32(), valuePair.positional().int32());
  }

  {
    // 先手が駒を取る手
    std::string src =
"P1-KY-KE-GI-KI-OU * -GI-KE-KY\n"
"P2 * -HI *  *  *  * -KI-KA * \n"
"P3-FU * -FU-FU-FU-FU-FU * -FU\n"
"P4 *  *  *  *  *  *  * -FU * \n"
"P5 * -FU *  *  *  *  *  *  * \n"
"P6 *  *  *  *  *  *  *  *  * \n"
"P7+FU+FU+FU+FU+FU+FU+FU * +FU\n"
"P8 * +KA+KI *  *  *  * +HI * \n"
"P9+KY+KE+GI * +OU+KI+GI+KE+KY\n"
"P+\n"
"P-00FU\n"
"+\n";
    std::istringstream iss(src);
    Board board;
    CsaReader::readBoard(iss, board);
    Move move(Piece::Rook, S28, S24, false);

    auto prevValuePair = eval.evaluate(board);
    board.makeMove(move);
    auto valuePair = eval.evaluateDiff(board, prevValuePair, move);
    auto correctValuePair = eval.evaluate(board);

    ASSERT_EQ(correctValuePair.material().int32(), valuePair.material().int32());
    ASSERT_EQ(correctValuePair.positional().int32(), valuePair.positional().int32());
  }

  {
    // 先手が駒を取りながら成る手
    std::string src =
"P1-KY-KE * -KI-OU-KI-GI-KE-KY\n"
"P2 * -HI * -GI *  *  * -KA * \n"
"P3-FU * -FU-FU-FU-FU * -FU-FU\n"
"P4 *  * +FU *  *  * -FU *  * \n"
"P5 * -FU *  *  *  *  *  *  * \n"
"P6 *  *  *  *  *  *  *  *  * \n"
"P7+FU+FU * +FU+FU+FU+FU+FU+FU\n"
"P8 * +KA+HI *  *  *  *  *  * \n"
"P9+KY+KE+GI+KI+OU+KI+GI+KE+KY\n"
"P+\n"
"P-\n"
"+\n";
    std::istringstream iss(src);
    Board board;
    CsaReader::readBoard(iss, board);
    Move move(Piece::Pawn, S74, S73, true);

    auto prevValuePair = eval.evaluate(board);
    board.makeMove(move);
    auto valuePair = eval.evaluateDiff(board, prevValuePair, move);
    auto correctValuePair = eval.evaluate(board);

    ASSERT_EQ(correctValuePair.material().int32(), valuePair.material().int32());
    ASSERT_EQ(correctValuePair.positional().int32(), valuePair.positional().int32());
  }

  {
    // 盤上の駒を動かす後手の手
    std::string src =
"P1-KY-KE-GI-KI-OU * -GI-KE-KY\n"
"P2 *  *  *  *  *  * -KI-KA * \n"
"P3-FU * -FU-FU-FU-FU-FU-FU-FU\n"
"P4 *  *  *  *  *  *  *  *  * \n"
"P5 *  *  *  *  *  *  * +FU * \n"
"P6 *  *  *  *  *  *  *  *  * \n"
"P7+FU-RY+FU+FU+FU+FU+FU+GI+FU\n"
"P8 * +KA+KI *  *  *  * +HI * \n"
"P9+KY+KE+GI * +OU+KI * +KE+KY\n"
"P+\n"
"P-00FU00FU\n"
"-\n";
    std::istringstream iss(src);
    Board board;
    CsaReader::readBoard(iss, board);
    Move move(Piece::Dragon, S87, S84, false);

    auto prevValuePair = eval.evaluate(board);
    board.makeMove(move);
    auto valuePair = eval.evaluateDiff(board, prevValuePair, move);
    auto correctValuePair = eval.evaluate(board);

    ASSERT_EQ(correctValuePair.material().int32(), valuePair.material().int32());
    ASSERT_EQ(correctValuePair.positional().int32(), valuePair.positional().int32());
  }

  {
    // 後手が駒を打つ手
    std::string src =
"P1-KY-KE-GI-KI-OU-KI-GI-KE-KY\n"
"P2 * -HI *  *  *  *  *  *  * \n"
"P3-FU-FU-FU-FU-FU-FU * -FU-FU\n"
"P4 *  *  *  *  *  * -FU *  * \n"
"P5 *  *  *  *  *  *  *  *  * \n"
"P6 *  * +FU *  *  *  * +FU * \n"
"P7+FU+FU * +FU+FU+FU+FU * +FU\n"
"P8 * +GI *  *  *  *  * +HI * \n"
"P9+KY+KE * +KI+OU+KI+GI+KE+KY\n"
"P+00KA\n"
"P-00KA\n"
"-\n";
    std::istringstream iss(src);
    Board board;
    CsaReader::readBoard(iss, board);
    Move move(Piece::Bishop, S65);

    auto prevValuePair = eval.evaluate(board);
    board.makeMove(move);
    auto valuePair = eval.evaluateDiff(board, prevValuePair, move);
    auto correctValuePair = eval.evaluate(board);

    ASSERT_EQ(correctValuePair.material().int32(), valuePair.material().int32());
    ASSERT_EQ(correctValuePair.positional().int32(), valuePair.positional().int32());
  }

  {
    // 後手が駒を取る手
    std::string src =
"P1-KY-KE-GI-KI-OU-KI-GI * -KY\n"
"P2 *  *  *  *  *  *  * -HI * \n"
"P3-FU-FU-FU-FU-FU+UM * -FU-FU\n"
"P4 *  *  *  *  *  * -FU *  * \n"
"P5 *  *  *  *  * -KE * +FU * \n"
"P6 *  * +FU *  *  *  *  *  * \n"
"P7+FU+FU * +FU+FU+FU+FU * +FU\n"
"P8 *  *  *  *  *  *  * +HI * \n"
"P9+KY+KE+GI+KI+OU+KI+GI+KE+KY\n"
"P+00FU\n"
"P-00KA\n"
"-\n";
    std::istringstream iss(src);
    Board board;
    CsaReader::readBoard(iss, board);
    Move move(Piece::Knight, S45, S57, false);

    auto prevValuePair = eval.evaluate(board);
    board.makeMove(move);
    auto valuePair = eval.evaluateDiff(board, prevValuePair, move);
    auto correctValuePair = eval.evaluate(board);

    ASSERT_EQ(correctValuePair.material().int32(), valuePair.material().int32());
    ASSERT_EQ(correctValuePair.positional().int32(), valuePair.positional().int32());
  }

  {
    // 後手が駒を取りながら成る手
    std::string src =
"P1-KY-KE-GI-KI-OU-KI * -KE-KY\n"
"P2 *  *  *  *  *  *  * -HI * \n"
"P3-FU-FU-FU-FU-FU-GI *  * -FU\n"
"P4 *  *  *  *  * -FU-FU *  * \n"
"P5 *  *  *  *  *  *  *  *  * \n"
"P6 *  * +FU * +FU *  *  *  * \n"
"P7+FU+FU * +FU * +FU+FU * +FU\n"
"P8 *  * +OU * +KI+GI * +HI * \n"
"P9+KY+KE+KA+KI *  *  * +KE+KY\n"
"P+00KA00FU\n"
"P-00GI00FU\n"
"-\n";
    std::istringstream iss(src);
    Board board;
    CsaReader::readBoard(iss, board);
    Move move(Piece::Rook, S22, S28, true);

    auto prevValuePair = eval.evaluate(board);
    board.makeMove(move);
    auto valuePair = eval.evaluateDiff(board, prevValuePair, move);
    auto correctValuePair = eval.evaluate(board);

    ASSERT_EQ(correctValuePair.material().int32(), valuePair.material().int32());
    ASSERT_EQ(correctValuePair.positional().int32(), valuePair.positional().int32());
  }

}

TEST(EvaluatorTest, testEstimate) {

  Evaluator eval(Evaluator::InitType::Zero);

  {
    std::string src =
"P1-KY-KE-GI-KI-OU-KI-GI * -KY\n"
"P2 *  *  *  *  *  *  * -HI * \n"
"P3-FU-FU-FU-FU-FU+UM * -FU-FU\n"
"P4 *  *  *  *  *  * -FU *  * \n"
"P5 *  *  *  *  * -KE * +FU * \n"
"P6 *  * +FU *  *  *  *  *  * \n"
"P7+FU+FU * +FU+FU+FU+FU * +FU\n"
"P8 *  *  *  *  *  *  * +HI * \n"
"P9+KY+KE+GI+KI+OU+KI+GI+KE+KY\n"
"P+00FU\n"
"P-00KA\n"
"-\n";
    std::istringstream iss(src);
    Board board;
    CsaReader::readBoard(iss, board);

    // 57桂不成
    Move move(Piece::Knight, S45, S57, false);
    Value value = eval.estimate(board, move);
    ASSERT_EQ(value.int32(), material::PawnEx);

    // 57桂成
    move = Move(Piece::Knight, S45, S57, true);
    value = eval.estimate(board, move);
    ASSERT_EQ(value.int32(),
        material::PawnEx
        + material::Pro_knight
        - material::Knight);

    // 55角
    move = Move(Piece::Bishop, S55);
    value = eval.estimate(board, move);
    ASSERT_EQ(value.int32(), 0);

    // 62玉
    move = Move(Piece::King, S51, S62, false);
    value = eval.estimate(board, move);
    ASSERT_EQ(value.int32(), 0);
  }

}

TEST(EvaluatorTest, testSymmetrize) {
  ASSERT_EQ(KKP_HROOK + 2, symmetrizeKkpIndex(KKP_HROOK + 2));
  ASSERT_EQ(KKP_BPAWN + 64, symmetrizeKkpIndex(KKP_BPAWN + 0));
  ASSERT_EQ(KKP_BPAWN + 29, symmetrizeKkpIndex(KKP_BPAWN + 45));
}

#endif // !defined(NDEBUG)
