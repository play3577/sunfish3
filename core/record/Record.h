/* Record.h
 *
 * Kubo Ryosuke
 */

#ifndef __SUNFISH_RECORD__
#define __SUNFISH_RECORD__

#include "../board/Board.h"
#include "../move/Move.h"
#include <vector>
#include <string>

namespace sunfish {

	struct RecordInfo {
		std::string title;
		std::string blackName;
		std::string whiteName;
		int timeLimitHour;
		int timeLimitMinutes;
		int timeLimitReadoff;
	};

	class Record {
	private:

		Board _board;
		std::vector<Move> _moves;
		unsigned _count;

	public:

		Record() : _count(0) {
		}
		Record(const Board& board) : _board(board), _count(0) {
		}
		Record(Board&& board) : _board(std::move(board)), _count(0) {
		}

		void init(const Board& board) {
			_board = board;
			_moves.clear();
			_moves.shrink_to_fit();
			_count = 0;
		}
		void init(Board::Handicap handicap) {
			_board.init(handicap);
			_moves.clear();
			_moves.shrink_to_fit();
			_count = 0;
		}

		/**
		 * 指定した手で1手進めます。
		 */
		bool makeMove(const Move& move);

		/**
		 * 1手進めます。
		 */
		bool makeMove();

		/**
		 * 1手戻します。
		 */
		bool unmakeMove();

		/**
		 * 現在の局面を取得します。
		 */
		const Board& getBoard() const {
			return _board;
		}

		/**
		 * 初期局面を取得します。
		 */
		Board getInitialBoard() const;

		/**
		 * 先手番かチェックします。
		 */
		bool isBlack() const {
			return _board.isBlack();
		}

		/**
		 * 先手番かチェックします。
		 */
		bool isBlackAt(int i) const {
			return isBlack() ^ (((int)_count - i) % 2 != 0);
		}

		/**
		 * 後手番かチェックします。
		 */
		bool isWhite() const {
			return !isBlack();
		}

		/**
		 * 後手番かチェックします。
		 */
		bool isWhiteAt(int i) const {
			return !isBlackAt(i);
		}

		/**
		 * 指し手を返します。
		 */
		Move getMoveAt(int i) const {
			return _moves[i];
		}

		/**
		 * 指し手を返します。
		 */
		Move getMove() const {
			return _count >= 1 ? getMoveAt(_count-1) : Move::empty();
		}

		/**
		 * 次の指し手を返します。
		 */
		Move getNextMove() const {
			return _count < getTotalCount() ? getMoveAt(_count) : Move::empty();
		}

		/**
		 * 総手数を返します。
		 */
		unsigned getTotalCount() const {
			return (unsigned)_moves.size();
		}

		/**
		 * 現在の手数を返します。
		 */
		unsigned getCount() const {
			return _count;
		}

	};

}

#endif //__SUNFISH_RECORD__
