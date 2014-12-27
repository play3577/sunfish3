/*
 * ConsoleManager.h
 *
 * Kubo Ryosuke
 */

#ifndef CONTROLLER_H_
#define CONTROLLER_H_

#include "core/record/Record.h"
#include "searcher/Searcher.h"

namespace sunfish {

	/**
	 * コンソールコマンド
	 */
	enum class Command {
		Empty = 0,
		Quit,
		Help,
		Prev,
		Next,
		Top,
		End,
		Search,

		__num__,

		Unknown,
	};

	/**
	 * ConsoleManager
	 */
	class ConsoleManager {
	private:

		enum class CommandResult {
			Quit, Changed, None
		};

		struct Config {
			bool autoBlack;
			bool autoWhite;
			int maxDepth;
			std::string inFileName;
			std::string outFileName;
		} _config;

		Searcher _searcher;
		Record _record;
		Command _prevCommand;

		void initConfig();

		void showHelp();

		Command parseCommand(const char* str);

		bool inputMove(const char* str, const Board& board, Move& move) const;

		void printBoard(const Board& board) const;

		bool isAuto() const {
			return (_record.getBoard().isBlack() && _config.autoBlack)
					|| (_record.getBoard().isWhite() && _config.autoWhite);
		}

		bool search(bool withMakeMove);

		CommandResult inputCommand();

	public:

		ConsoleManager() {
			initConfig();
			_prevCommand = Command::Empty;
		}

		void setAutoBlack(bool enable) {
			_config.autoBlack = enable;
		}
		bool getAutoBlack() const {
			return _config.autoBlack;
		}

		void setAutoWhite(bool enable) {
			_config.autoWhite = enable;
		}
		bool getAutoWhite() const {
			return _config.autoWhite;
		}

		void setMaxDepth(int depth) {
			_config.maxDepth = depth;
		}
		bool getMaxDepth() const  {
			return _config.maxDepth;
		}

		void setInFileName(const std::string& inFileName) {
			_config.inFileName = inFileName;
		}
		const std::string& getInFileName() const {
			return _config.inFileName;
		}

		void setOutFileName(const std::string& outFileName) {
			_config.outFileName = outFileName;
		}
		const std::string& getOutFileName() const {
			return _config.outFileName;
		}

		bool play();

	};
}

#endif // CONTROLLER_H_
