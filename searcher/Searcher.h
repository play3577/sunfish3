/* Searcher.h
 *
 * Kubo Ryosuke
 */

#ifndef __SUNFISH_SEARCHER__
#define __SUNFISH_SEARCHER__

#include "mate/Mate.h"
#include "mate/MateHistory.h"
#include "SearchInfo.h"
#include "eval/EvaluateTable.h"
#include "see/SeeTable.h"
#include "tree/Tree.h"
#include "history/History.h"
#include "tt/TT.h"
#include "time/TimeManager.h"
#include "core/record/Record.h"
#include "core/util/Timer.h"
#include <mutex>
#include <atomic>

namespace sunfish {

	class Tree;
	struct Worker;

	class Gains {
	private:

		Value _gains[Piece::Num][Position::N];

	public:

		void clear() {
			memset(&_gains[0][0], 0, sizeof(_gains));
		}

		void update(const Move& move, Value gain) {
			assert(move.piece() < Piece::Num);
			assert(move.to() >= 0);
			assert(move.to() < Position::N);
			Value& ref = _gains[move.piece()][move.to()];
			ref = Value::max(ref - 1, gain);
		}

		Value get(const Move& move) {
			assert(move.piece() < Piece::Num);
			assert(move.to() >= 0);
			assert(move.to() < Position::N);
			Value g = _gains[move.piece()][move.to()];
			return g;
		}

	};

	class Searcher {
	public:

		static CONSTEXPR int Depth1Ply = 8;

		/** 探索設定 */
		struct Config {
			int maxDepth;
			int treeSize;
			int workerSize;
			bool limitEnable;
			double limitSeconds;
			bool ponder;
		};

		static const int DefaultMaxDepth = 7;

	private:

		Config _config;
		SearchInfo _info;
		Timer _timer;

		/** tree */
		Tree* _trees;

		/** worker */
		Worker* _workers;

		/** 評価関数 */
		Evaluator _eval;

		/** history heuristic */
		History _history;

		/** transposition table */
		TT _tt;

		/** gains */
		Gains _gains;

		/** mate table */
		MateTable _mateTable;

		/** mate history */
		MateHistory _mateHistory;

		/** record */
		std::vector<Move> _record;

		/** values of child node of root node */
		int _rootValues[1024];

		int _rootDepth;

		SeeTable<22> _seeCache;

		std::mutex _splitMutex;

		/** 中断フラグ */
		std::atomic<bool> _forceInterrupt;

		/** 実行中フラグ */
		std::atomic<bool> _isRunning;

		/** 思考時間制御 */
		TimeManager _timeManager;

		/**
		 * 設定の初期化
		 */
		void initConfig() {
			_config.maxDepth = DefaultMaxDepth;
			_config.treeSize = 1;
			_config.workerSize = 1;
			_config.limitEnable = true;
			_config.limitSeconds = 10.0;
			_config.ponder = false;
		}

		/**
		 * tree の確保
		 */
		void allocateTrees();

		/**
		 * worker の確保
		 */
		void allocateWorkers();

		/**
		 * tree の再確保
		 */
		void reallocateTrees();

		/**
		 * worker の再確保
		 */
		void reallocateWorkers();

		/**
		 * tree の解放
		 */
		void releaseTrees();

		/**
		 * worker の解放
		 */
		void releaseWorkers();

		/**
		 * worker の取得
		 */
		Worker& getWorker(Tree& tree);

		void mergeInfo();

		/**
		 * 前処理
		 */
		void before(const Board& initialBoard);

		/**
		 * 後処理
		 */
		void after();

		/**
		 * 探索中断判定
		 */
		bool isInterrupted(Tree& tree);

		/**
		 * get see value
		 */
		template <bool shallow>
		Value searchSee(const Board& board, const Move& move, Value alpha, Value beta);

		/**
		 * sort moves by see
		 */
		void sortSee(Tree& tree, int offset, Value standPat, Value alpha, bool enableKiller, bool estimate, bool exceptSmallCapture, bool isQuies);

		/**
		 * except prior moves
		 */
		void exceptPriorMoves(Tree& tree);

		/**
		 * pick best move by history
		 */
		bool pickOneHistory(Tree& tree);

		/**
		 * sort moves by history
		 */
		void sortHistory(Tree& tree);

		/**
		 * update history
		 */
		void updateHistory(Tree& tree, int depth, const Move& move);

		/**
		 * get LMR depth
		 */
		int getReductionDepth(bool improving, int depth, int moveCount, const Move& move, bool isNullWindow);

		/**
		 * get next move
		 */
		bool nextMove(Tree& tree);

		/**
		 * get next move
		 */
		bool nextMoveQuies(Tree& tree, int ply, Value standPat, Value alpha);

		/**
		 * store PV-nodes to TT
		 */
		void storePv(Tree& tree, const Pv& pv, int ply);

		bool isNeedMateSearch(Tree& tree, bool black, int depth);

		void updateMateHistory(Tree& tree, bool black, bool mate);

		/**
		 * quiesence search
		 */
		Value qsearch(Tree& tree, bool black, int qply, Value alpha, Value beta);

		/**
		 * update killer move
		 */
		void updateKiller(Tree& tree, const Move& move);

		/**
		 * nega-max search
		 */
		Value search(Tree& tree, bool black, int depth, Value alpha, Value beta, NodeStat stat = NodeStat::Default);

		/**
		 * split
		 */
		bool split(Tree& parent, bool black, int depth, Value alpha, Value beta, Move best, Value standPat, NodeStat stat, bool improving);

		void searchTlp(Tree& tree);

		void shutdownSiblings(Tree& parent);

		/**
		 * search on root node
		 */
		Value searchRoot(Tree& tree, int depth, Value alpha, Value beta, Move& best, bool forceFullWindow = false);

		/**
		 * aspiration search
		 * @return {負けたか中断された場合にfalseを返します。}
		 */
		bool searchAsp(int depth, Move& best, Value* pval = nullptr);

		void showPv(int depth, const Pv& pv, const Value& value);

		void showEndOfIterate(int depth);

		void generateMovesOnRoot();

		/**
		 * iterative deepening search from root node
		 * @return {負けたか深さ1で中断された場合にfalseを返します。}
		 */
		bool idsearch(Move& best);

	public:

		std::atomic<int> _idleTreeCount;
		std::atomic<int> _idleWorkerCount;

		/**
		 * コンストラクタ
		 */
		Searcher();

		/**
		 * デストラクタ
		 */
		~Searcher();

		/**
		 * 設定を反映します。
		 */
		void setConfig(const Config& config) {
			auto org = _config;
			_config = config;
			if (_config.treeSize != org.treeSize) {
				reallocateTrees();
			}
			if (_config.workerSize != org.workerSize) {
				reallocateWorkers();
			}
		}

		/**
		 * 設定を取得します。
		 */
		const Config& getConfig() const {
			return _config;
		}

		/**
		 * 探索情報を取得します。
		 */
		const SearchInfo& getInfo() const {
			return _info;
		}

		std::string getInfoString() const;

		/**
		 * SHEK と千日手検出のための過去の棋譜をクリアします。
		 */
		void clearRecord();

		/**
		 * SHEK と千日手検出のために過去の棋譜をセットします。
		 */
		void setRecord(const Record& record);

		/**
		 * 探索を強制的に打ち切ります。
		 */
		void forceInterrupt();

		/**
		 * 探索中かチェックします。
		 */
		bool isRunning() const {
			return _isRunning.load();
		}

		/**
		 * 指定した局面に対して探索を実行します。
		 * @return {負けたいか中断された場合にfalseを返します。}
		 */
		bool search(const Board& initialBoard, Move& best);

		/**
		 * 指定した局面に対して反復深化探索を実行します。
		 * @return {負けたか深さ1で中断された場合にfalseを返します。}
		 */
		bool idsearch(const Board& initialBoard, Move& best);

		/**
		 * Evaluator を取得します。
		 */
		Evaluator& getEvaluator() {
			return _eval;
		}

		/**
		 * TT をクリアします。
		 */
		void clearTT() {
			_tt.init();
		}

		/**
		 * historyをクリアします。
		 */
		void clearHistory() {
			_history.init();
		}

		std::mutex& getSplitMutex() {
			return _splitMutex;
		}

		void addIdleWorker() {
			_idleWorkerCount.fetch_add(1);
		}

		void reduceIdleWorker() {
			_idleWorkerCount.fetch_sub(1);
		}

		void releaseTree(int tid);

		void searchTlp(int tid) {
			auto& tree = _trees[tid];
			searchTlp(tree);
		}

		static int standardTreeSize(int workerSize) {
			return workerSize * 4 - 3;
		}

	};

}

#endif //__SUNFISH_SEARCHER__
