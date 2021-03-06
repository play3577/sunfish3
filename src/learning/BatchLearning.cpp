/* BatchLearning.cpp
 * 
 * Kubo Ryosuke
 */

#ifndef NLEARN

#include "BatchLearning.h"
#include "LearningConfig.h"
#include "LearningTemplates.h"
#include "config/Config.h"
#include "core/move/MoveGenerator.h"
#include "core/record/CsaReader.h"
#include "core/util/FileList.h"
#include "searcher/eval/Material.h"
#include <list>
#include <algorithm>
#include <cstdlib>

#define SEARCH_WINDOW  256
#define NORM           1.0e-2f

namespace {

using namespace sunfish;

bool isValidSquare(Piece piece, Square square) {
  if ((piece == Piece::BPawn || piece == Piece::BLance) && !square.isPawnMovable<true>()) {
    return false;
  }

  if ((piece == Piece::BKnight) && !square.isKnightMovable<true>()) {
    return false;
  }

  if ((piece == Piece::WPawn || piece == Piece::WLance) && !square.isPawnMovable<false>()) {
    return false;
  }

  if ((piece == Piece::WKnight) && !square.isKnightMovable<false>()) {
    return false;
  }

  return true;
}

std::string trainingDataFileName(uint32_t wn) {
  std::ostringstream oss;
  oss << "training" << wn << ".dat";
  return oss.str();
}

void setSearcherDepth(Searcher& searcher, int depth) {
  auto searchConfig = searcher.getConfig();
  searchConfig.maxDepth = depth;
  searcher.setConfig(searchConfig);
}

inline float gain() {
  return 7.0f / SEARCH_WINDOW;
}

inline float sigmoid(float x) {
  return 1.0 / (1.0 + std::exp(x * -gain()));
}

inline float dsigmoid(float x) {
  float s = sigmoid(x);
  return (s - s * s) * gain();
}

inline float loss(float x) {
  return sigmoid(x);
}

inline float gradient(float x) {
  return dsigmoid(x);
}

inline float norm(float x) {
  if (x > 0.0f) {
    return -NORM;
  } else if (x < 0.0f) {
    return NORM;
  } else {
    return 0.0f;
  }
}

} // namespace

namespace sunfish {

/**
 * プログレスバーの表示を更新します。
 */
void BatchLearning::updateProgress() {
  int cmax = 50;

  std::cout << "\r";
  for (int c = 0; c < cmax; c++) {
    if (c * totalJobs_ <= cmax * completedJobs_) {
      std::cout << '#';
    } else {
      std::cout << ' ';
    }
  }
  float percentage = (float)completedJobs_ / totalJobs_ * 100.0f;
  std::cout << " [" << percentage << "%]";
  std::cout << std::flush;
}

/**
 * プログレスバーの表示を終了します。
 */
void BatchLearning::closeProgress() {
  std::cout << "\n";
  std::cout << std::flush;
}

/**
 * ジョブを拾います。
 */
void BatchLearning::work(uint32_t wn) {
  while (!shutdown_) {
    std::this_thread::sleep_for(std::chrono::milliseconds(1));

    Job job;

    // dequeue
    {
      std::lock_guard<std::mutex> lock(mutex_);
      if (jobQueue_.empty()) {
        continue;
      }
      job = jobQueue_.front();
      // worker number の制約がある場合は一致するものだけを拾う
      if (job.wn != InvalidWorkerNumber && job.wn != wn) {
        continue;
      }
      jobQueue_.pop();
      activeCount_++;
    }

    job.method(wn);

    completedJobs_++;
    activeCount_--;

    if (job.type == JobType::GenerateTrainingData) {
      std::lock_guard<std::mutex> lock(mutex_);
      updateProgress();
    }
  }
}

/**
 * ワーカーがジョブを終えるまで待機します。
 */
void BatchLearning::waitForWorkers() {
  while (true) {
    {
      std::lock_guard<std::mutex> lock(mutex_);
      if (jobQueue_.empty() && activeCount_ == 0) {
        return;
      }
    }
    std::this_thread::sleep_for(std::chrono::milliseconds(1));
  }
}

void BatchLearning::generateGradientX() {
  SQUARE_EACH(king1) {
    for (int i = 0; i < KKP_MAX; i++) {
      float gb = 0.0f;
      float gw = 0.0f;
      SQUARE_EACH(king2) {
        gb += g_.t_->kkp[king1.index()][king2.index()][i];
        gw += g_.t_->kkp[king2.index()][king1.index()][i];
      }
      gx_.t_->kpb[king1.index()][i] = gb;
      gx_.t_->kpw[king1.index()][i] = gw;
    }
  }
}

void BatchLearning::mergeParametersX() {
  evalMerged_ = eval_;

  SQUARE_EACH(king1) {
    for (int i = 0; i < KKP_MAX; i++) {
      const float eb = ex_.t_->kpb[king1.index()][i];
      const float ew = ex_.t_->kpw[king1.index()][i];
      SQUARE_EACH(king2) {
        evalMerged_.t_->kkp[king1.index()][king2.index()][i] += eb;
        evalMerged_.t_->kkp[king2.index()][king1.index()][i] += ew;
      }
    }
  }
}

/**
 * 訓練データを生成します。
 */
void BatchLearning::generateTrainingData(uint32_t wn, Board board, Move move0) {
  int depth = config_.getInt(LCONF_DEPTH);

  // 合法手生成
  Moves moves;
  MoveGenerator::generate(board, moves);

  if (moves.size() < 2) {
    return;
  }

  Value val0;
  Move tmpMove;
  std::list<PV> list;

  auto& to = threadObjects_[wn];
  auto& searcher = *(to.searcher);
  auto& outTrainingData = to.outTrainingData;

  // ヒストリのクリア
  searcher.clearHistory();

  {
    int newDepth = depth;
    if (board.isCheck(move0)) {
      newDepth += 1;
    }

    // 探索
    board.makeMove(move0);
    setSearcherDepth(searcher, newDepth);
    searcher.search(board, tmpMove);
    board.unmakeMove(move0);

    // PV と評価値
    const auto& info = searcher.getInfo();
    const auto& pv = info.pv;
    val0 = -info.eval;

    // 詰みは除外
    if (val0 <= -Value::Mate || val0 >= Value::Mate) {
      return;
    }

    list.push_back({});
    list.back().set(move0, 0, pv);
  }

  totalMoves_++;

  // 棋譜の手の評価値から window を決定
  Value alpha = val0 - SEARCH_WINDOW;
  Value beta = val0 + SEARCH_WINDOW;

  for (auto& move : moves) {
    if (move == move0) {
      continue;
    }

    int newDepth = depth;
    if (board.isCheck(move)) {
      newDepth += 1;
    }

    // 探索
    bool valid = board.makeMove(move);
    if (!valid) { continue; }
    setSearcherDepth(searcher, newDepth);
    searcher.search(board, tmpMove, -beta, -alpha, true);
    board.unmakeMove(move);

    // PV と評価値
    const auto& info = searcher.getInfo();
    const auto& pv = info.pv;
    Value val = -info.eval;

    if (val <= alpha) {
      continue;
    }

    if (val >= beta) {
      oowLoss_++;
      continue;
    }

    list.push_back({});
    list.back().set(move, 0, pv);
  }

  // 書き出し
  if (!list.empty()) {
    std::lock_guard<std::mutex> lock(mutex_);

    // ルート局面
    CompactBoard cb = board.getCompactBoard();
    outTrainingData->write(reinterpret_cast<char*>(&cb), sizeof(cb));

    for (const auto& pv : list) {
      // 手順の長さ
      uint8_t length = static_cast<uint8_t>(pv.size()) + 1;
      outTrainingData->write(reinterpret_cast<char*>(&length), sizeof(length));

      // 手順
      for (int i = 0; i < pv.size(); i++) {
        uint16_t m = Move::serialize16(pv.get(i).move);
        outTrainingData->write(reinterpret_cast<char*>(&m), sizeof(m));
      }
    }

    // 終端
    uint8_t n = 0;
    outTrainingData->write(reinterpret_cast<char*>(&n), sizeof(n));
  }
}

/**
 * 訓練データを生成します。
 */
void BatchLearning::generateTrainingDataOnWorker(uint32_t wn, const std::string& path) {
  Record record;
  if (!CsaReader::read(path, record)) {
    Loggers::error << "Could not read csa file. [" << path << "]";
    exit(1);
  }

  // 棋譜の先頭へ
  while (record.unmakeMove())
    ;

  while (true) {
    // 次の1手を取得
    Move move = record.getNextMove();
    if (move.isEmpty()) {
      break;
    }

    generateTrainingData(wn, record.getBoard(), move);

    // 1手進める
    if (!record.makeMove()) {
      break;
    }
  }
}

/**
 * 訓練データ作成を開始します。
 */
bool BatchLearning::generateTrainingData() {
  // open training data files
  for (uint32_t wn = 0; wn < nt_; wn++) {
    auto& to = threadObjects_[wn];
    auto& outTrainingData = to.outTrainingData;
    std::string fname = trainingDataFileName(wn);

    outTrainingData.reset(new std::ofstream);
    outTrainingData->open(fname, std::ios::binary | std::ios::out);

    if (!*outTrainingData) {
      Loggers::error << "open error!! [" << fname << "]";
      return false;
    }
  }

  // enumerate .csa files
  FileList fileList;
  std::string dir = config_.getString(LCONF_KIFU);
  fileList.enumerate(dir.c_str(), "csa");

  if (fileList.size() == 0) {
    Loggers::error << "no files.";
    return false;
  }

  completedJobs_ = 0;
  totalJobs_ = fileList.size();

  {
    // push jobs
    std::lock_guard<std::mutex> lock(mutex_);
    for (const auto& path : fileList) {
      using namespace std::placeholders;
      jobQueue_.push({
        JobType::GenerateTrainingData,
        std::bind(std::mem_fn(&BatchLearning::generateTrainingDataOnWorker), this, _1, path),
        InvalidWorkerNumber
      });
    }
  }

  waitForWorkers();

  // close progress bar
  closeProgress();

  // close training data files
  size_t size = 0;
  for (uint32_t wn = 0; wn < nt_; wn++) {
    auto& to = threadObjects_[wn];
    auto& outTrainingData = to.outTrainingData;
    size += outTrainingData->tellp();
    outTrainingData->close();
  }
  Loggers::message << "training_data_size=" << size;

  return true;
}

/**
 * 勾配ベクトルを生成します。
 */
bool BatchLearning::generateGradient(uint32_t wn) {
  std::string fname = trainingDataFileName(wn);
  std::ifstream inTrainingData(fname);
  if (!inTrainingData) {
    Loggers::error << "open error!! [" << fname << "]";
    return false;
  }

  float loss0 = 0.0f;
  std::unique_ptr<FVM> gm0(new FVM);
  std::unique_ptr<FV> g0(new FV);

  gm0->init();
  g0->init();

  while (true) {
    // ルート局面
    CompactBoard cb;
    inTrainingData.read(reinterpret_cast<char*>(&cb), sizeof(cb));
    if (inTrainingData.eof()) {
      break;
    }

    const Board root(cb);
    const bool black = root.isBlack();

    auto readPV = [&inTrainingData](Board& board) {
      // 手順の長さ
      uint8_t length;
      inTrainingData.read(reinterpret_cast<char*>(&length), sizeof(length));
      if (length == 0) {
        return false;
      }
      length--;

      // 手順
      bool ok = true;
      for (uint8_t i = 0; i < length; i++) {
        uint16_t m;
        inTrainingData.read(reinterpret_cast<char*>(&m), sizeof(m));
        Move move = Move::deserialize16(m, board);
        if (!ok || move.isEmpty() || !board.makeMove(move)) {
          ok = false;
        }
      }

      return true;
    };

    Board board0 = root;
    readPV(board0);
    Value val0 = evalMerged_.evaluate(board0).value();

    while (true) {
      Board board = root;
      if (!readPV(board)) {
        break;
      }
      Value val = evalMerged_.evaluate(board).value();

      float diff = val.int32() - val0.int32();
      diff = black ? diff : -diff;

      float g = gradient(diff);
      g = black ? g : -g;

      loss0 += loss(diff);
      gm0->extract(board0, g);
      gm0->extract(board, -g);
      g0->extract<float, true>(board0, g);
      g0->extract<float, true>(board, -g);
    }
  }

  inTrainingData.close();

  {
    std::lock_guard<std::mutex> lock(mutex_);

    loss_ += loss0;

    gm_.pawn       += gm0->pawn;
    gm_.lance      += gm0->lance;
    gm_.knight     += gm0->knight;
    gm_.silver     += gm0->silver;
    gm_.gold       += gm0->gold;
    gm_.bishop     += gm0->bishop;
    gm_.rook       += gm0->rook;
    gm_.tokin      += gm0->tokin;
    gm_.pro_lance  += gm0->pro_lance;
    gm_.pro_knight += gm0->pro_knight;
    gm_.pro_silver += gm0->pro_silver;
    gm_.horse      += gm0->horse;
    gm_.dragon     += gm0->dragon;

    for (size_t i = 0; i < FV::size(); i++) {
      ((FV::ValueType*)g_.t_)[i] += ((FV::ValueType*)g0->t_)[i];
    }
  }

  return true;
}

/**
 * 勾配ベクトルを生成します。
 */
bool BatchLearning::generateGradient() {
  std::atomic<bool> ok(true);

  gm_.init();
  g_.init();

  {
    std::lock_guard<std::mutex> lock(mutex_);
    for (uint32_t wn = 0; wn < nt_; wn++) {
      jobQueue_.push({
        JobType::GenerateGradient,
        [this, &ok](uint32_t wn) {
          ok = generateGradient(wn) && ok;
        },
        wn
      });
    }
  }

  waitForWorkers();

  generateGradientX();

  return ok;
}

void BatchLearning::updateParameter(uint32_t wn,
    FV::ValueType& g, Evaluator::ValueType& e) {
  auto& to = threadObjects_[wn];
  auto& r = *(to.rand);

  g += norm(e);
  if (g > 0.0f) {
    e += r.getBit() + r.getBit();
  } else if (g < 0.0f) {
    e -= r.getBit() + r.getBit();
  }
};

/**
 * パラメータを更新します。
 */
void BatchLearning::updateParameters(uint32_t wn) {
  int begin = wn;
  int step = nt_;

  for (size_t i = begin; i < FV::size(); i += step) {
    updateParameter(wn, ((FV::ValueType*)g_.t_)[i],
      ((Evaluator::ValueType*)eval_.t_)[i]);
  }

  for (size_t i = begin; i < FVX::size(); i += step) {
    updateParameter(wn, ((FVX::ValueType*)gx_.t_)[i],
      ((EvaluatorX::ValueType*)ex_.t_)[i]);
  }
}

/**
 * パラメータを更新します。
 */
void BatchLearning::updateParameters() {
  LearningTemplates::symmetrize(g_, [](float& a, float& b) {
      a = b = a + b;
  });

  updateMaterial();

  {
    std::lock_guard<std::mutex> lock(mutex_);
    for (uint32_t wn = 0; wn < nt_; wn++) {
      jobQueue_.push({
        JobType::UpdateParam,
        [this](uint32_t wn) {
          updateParameters(wn);
        },
        wn
      });
    }
  }

  waitForWorkers();

  LearningTemplates::symmetrize(eval_, [](Evaluator::ValueType& a, Evaluator::ValueType& b) {
      a = b;
  });

  LearningTemplates::symmetrize(ex_, [](Evaluator::ValueType& a, Evaluator::ValueType& b) {
      a = b;
  });

  mergeParametersX();

  max_ = 0;
  magnitude_ = 0;
  nonZero_ = 0;
  for (size_t i = 0; i < Evaluator::size(); i++) {
    Evaluator::ValueType e = ((Evaluator::ValueType*)evalMerged_.t_)[i];
    max_ = std::max(max_, (Evaluator::ValueType)std::abs(e));
    magnitude_ += std::abs(e);
    nonZero_ += e != 0 ? 1 : 0;
  }

  // ハッシュ表を初期化
  evalMerged_.clearCache();
  // transposition table は SearchConfig::learning で無効にしている
  //for (uint32_t wn = 0; wn < nt_; wn++) {
  //  searchers_[wn]->clearTT();
  //}
}

/**
 * 駒割りを更新します。
 */
void BatchLearning::updateMaterial() {
  float* p[13];

  p[0]  = &gm_.pawn;
  p[1]  = &gm_.lance;
  p[2]  = &gm_.knight;
  p[3]  = &gm_.silver;
  p[4]  = &gm_.gold;
  p[5]  = &gm_.bishop;
  p[6]  = &gm_.rook;
  p[7]  = &gm_.tokin;
  p[8]  = &gm_.pro_lance;
  p[9]  = &gm_.pro_knight;
  p[10] = &gm_.pro_silver;
  p[11] = &gm_.horse;
  p[12] = &gm_.dragon;

  // 昇順でソート
  std::sort(p, p + 13, [](float* a, float* b) {
    return (*a) < (*b);
  });

  auto& to = threadObjects_[0];

  // シャッフル
  to.rand->shuffle(p, p + 6);
  to.rand->shuffle(p + 6, p + 13);

  // 更新値を決定
  *p[0]  = *p[1]  = -2.0f;
  *p[2]  = *p[3]  = *p[4]  = -1.0f;
  *p[5]  = *p[6]  = *p[7]  = 0.0f;
  *p[8]  = *p[9]  = *p[10] = 1.0f;
  *p[11] = *p[12] = 2.0f;

  // 値を更新
  material::Pawn       += gm_.pawn;
  material::Lance      += gm_.lance;
  material::Knight     += gm_.knight;
  material::Silver     += gm_.silver;
  material::Gold       += gm_.gold;
  material::Bishop     += gm_.bishop;
  material::Rook       += gm_.rook;
  material::Tokin      += gm_.tokin;
  material::Pro_lance  += gm_.pro_lance;
  material::Pro_knight += gm_.pro_knight;
  material::Pro_silver += gm_.pro_silver;
  material::Horse      += gm_.horse;
  material::Dragon     += gm_.dragon;

  // 交換値を更新
  material::updateEx();
}

/**
 * バッチ学習の反復処理を実行します。
 */
bool BatchLearning::iterate() {
  const int iterateCount = config_.getInt(LCONF_ITERATION);
  int  updateCount = 128;

  for (int i = 0; i < iterateCount; i++) {
    totalMoves_ = 0;
    oowLoss_ = 0;

    if (!generateTrainingData()) {
      return false;
    }

    for (int j = 0; j < updateCount; j++) {
      loss_ = 0.0f;

      if (!generateGradient()) {
        return false;
      }

      updateParameters();

      float elapsed = timer_.get();
      float oowLoss = (float)oowLoss_ / totalMoves_;
      float totalLoss = ((float)oowLoss_ + loss_) / totalMoves_;

      Loggers::message
        << "elapsed=" << elapsed
        << "\titeration=" << i << "," << j
        << "\toow_loss=" << oowLoss
        << "\tloss=" << totalLoss
        << "\tmax=" << max_
        << "\tmagnitude=" << magnitude_
        << "\tnon_zero=" << nonZero_
        << "\tzero=" << (Evaluator::size() - nonZero_);
    }

    // 保存
    material::writeFile();
    evalMerged_.writeFile();

    updateCount = std::max(updateCount / 2, 16);
  }

  return true;
}

/**
 * 学習を実行します。
 */
bool BatchLearning::run() {
  Loggers::message << "begin learning";

  timer_.set();

  // 初期化
  evalMerged_.init();
  eval_.init();
  ex_.init();

  // 学習スレッド数
  nt_ = config_.getInt(LCONF_THREADS);

  // Searcher生成
  for (uint32_t wn = 0; wn < nt_; wn++) {
  }

  activeCount_ = 0;

  // ワーカースレッド生成
  shutdown_ = false;
  threadObjects_.clear();
  for (uint32_t wn = 0; wn < nt_; wn++) {
    threadObjects_.push_back(ThreadObject {
      std::thread(std::bind(std::mem_fn(&BatchLearning::work), this, wn)),
      std::unique_ptr<Searcher>(new Searcher(evalMerged_)),
      std::unique_ptr<Random>(new Random()),
      nullptr,
    });

    auto& to = threadObjects_.back();
    auto& searcher = *(to.searcher);

    auto searchConfig = searcher.getConfig();
    searchConfig.workerSize = 1;
    searchConfig.treeSize = Searcher::standardTreeSize(searchConfig.workerSize);
    searchConfig.enableLimit = false;
    searchConfig.enableTimeManagement = false;
    searchConfig.ponder = false;
    searchConfig.logging = false;
    searchConfig.learning = true;
    searcher.setConfig(searchConfig);
  }

  bool ok = iterate();

  // ワーカースレッド停止
  shutdown_ = true;
  for (uint32_t wn = 0; wn < nt_; wn++) {
    auto& to = threadObjects_[wn];
    to.thread.join();
  }

  if (!ok) {
    return false;
  }

  Loggers::message << "completed..";

  float elapsed = timer_.get();
  Loggers::message << "elapsed: " << elapsed;
  Loggers::message << "end learning";

  return true;
}

}

#endif // NLEARN
