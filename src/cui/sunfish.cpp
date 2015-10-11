/* sunfish.cpp
 * 
 * Kubo Ryosuke
 */

#include "sunfish.h"
#include "config.h"
#include "console/ConsoleManager.h"
#include "program_options/ProgramOptions.h"
#include "logger/Logger.h"
#include <iostream>
#include <string>

using namespace sunfish;

int play(const ConsoleManager::Config&);

// book.cpp
int generateBook(const std::string& directory);

// network.cpp
int network();

// learning.cpp
int learn();
int analyzeEvalBin();

// solve.cpp
int solve(const std::vector<std::string>& problems, const ConsoleManager::Config&);

// profile.cpp
int profile(const ConsoleManager::Config&, bool);

// test.cpp
int test();

// dev.cpp
int exprMoveGenSpeed();
int generateZobrist();
int checkMoveGen();

/**
 * entry point
 */
int main(int argc, char** argv, char** /*envp*/) {
  std::cerr << SUNFISH_NAME << ' ';
  std::cerr << SUNFISH_VERSION << '\n';
  std::cerr << SUNFISH_COPYRIGHT << '\n';
  std::cerr << std::endl;

  // program options
  ProgramOptions po;
  po.addOption("in", "i", "record path", true);
  po.addOption("out", "o", "record path", true);
  po.addOption("black", "b", "[auto/manual]", true);
  po.addOption("white", "w", "[auto/manual]", true);
  po.addOption("depth", "d", "max depth (default: 15)", true);
  po.addOption("time", "t", "max time for 1 move (default: 3)", true);
  po.addOption("worker", "r", "the number of worker threads", true);
  po.addOption("book", "generate book", true);
  po.addOption("network", "n", "network mode");
#ifndef NLEARN
  po.addOption("learn", "l", "learning");
  po.addOption("analyze", "a", "");
#endif // NLEARN
  po.addOption("problem", "p", "solve problems");
  po.addOption("profile", "solve problems");
  po.addOption("profile1", "solve one problem");
#ifndef NDEBUG
  po.addOption("test", "unit test");
  po.addOption("dev", "development method", true);
#endif
  po.addOption("help", "h", "show this help.");
  po.parse(argc, argv);

  for (const auto& invalidArg : po.getInvalidArgs()) {
    std::cerr << "WARNING: `" << invalidArg.arg << "' is invalid argument: " << invalidArg.reason << std::endl;
  }

  if (po.has("help")) {
    // show help
    std::cerr << po.help() << std::endl;
    return 0;

  } else if (po.has("book")) {
    std::string directory = po.getValue("book");
    return generateBook(directory);

  } else if (po.has("network")) {
    return network();

#ifndef NLEARN
  } else if (po.has("learn")) {
    return learn();

  } else if (po.has("analyze")) {
    return analyzeEvalBin();

#endif // NLEARN
#ifndef NDEBUG
  } else if (po.has("test")) {
    // unit test
    return test();

  } else if (po.has("dev")) {
    // development
    std::string code = po.getValue("dev");

    if (code == "gen_speed_test") {
      return exprMoveGenSpeed();

    } else if (code == "zobrist") {
      return generateZobrist();

    } else if (code == "gen_check") {
      return checkMoveGen();

    } else {
      std::cerr << '"' << code << "\" is unknown code." << std::endl;
      return 1;

    }

#endif
  }

  auto config = ConsoleManager::getDefaultConfig();

  // 起動時に読み込む棋譜ファイル
  if (po.has("in")) {
    config.inFileName = po.getValue("in");
    config.autoBlack = false;
    config.autoWhite = false;
  }

  // 自動保存する棋譜ファイル
  if (po.has("out")) {
    config.outFileName = po.getValue("out");
  }

  // 先手番自動対局 or マニュアル
  if (po.has("black")) {
    std::string value = po.getValue("black");
    if (value == "auto") {
      config.autoBlack = true;
    } else if (value == "manual") {
      config.autoBlack = false;
    } else {
      std::cerr << value << " is unknown value for --black option." << std::endl;
    }
  }

  // 後手番自動対局 or マニュアル
  if (po.has("white")) {
    std::string value = po.getValue("white");
    if (value == "auto") {
      config.autoWhite = true;
    } else if (value == "manual") {
      config.autoWhite = false;
    } else {
      std::cerr << value << " is unknown value for --white option." << std::endl;
    }
  }

  // 最大探索深さ
  if (po.has("depth")) {
    int depth = std::stoi(po.getValue("depth"));
    config.maxDepth = depth;
  }

  // worker
  if (po.has("worker")) {
    int worker = std::stoi(po.getValue("worker"));
    config.worker = worker;
  }

  // 最大思考時間
  if (po.has("time")) {
    config.limitSeconds = std::stod(po.getValue("time"));
  }

  if (po.has("problem")) {
    // 問題解答
    return solve(po.getStdArgs(), config);
  }

  if (po.has("profile")) {
    // profiling
    return profile(config, true);
  }

  if (po.has("profile1")) {
    // profiling
    return profile(config, false);
  }

  return play(config);
}

/**
 * コンソール上での対局を行います。
 */
int play(const ConsoleManager::Config& config) {

  // init loggers
  Loggers::error.addStream(std::cerr, ESC_SEQ_COLOR_RED, ESC_SEQ_COLOR_RESET);
  Loggers::warning.addStream(std::cerr, ESC_SEQ_COLOR_YELLOW, ESC_SEQ_COLOR_RESET);
  Loggers::message.addStream(std::cerr);
  Loggers::send.addStream(std::cerr, true, true, ESC_SEQ_COLOR_BLUE, ESC_SEQ_COLOR_RESET);
  Loggers::receive.addStream(std::cerr, true, true, ESC_SEQ_COLOR_MAGENTA, ESC_SEQ_COLOR_RESET);
#ifndef NDEBUG
  Loggers::debug.addStream(std::cerr, ESC_SEQ_COLOR_CYAN, ESC_SEQ_COLOR_RESET);
  Loggers::test.addStream(std::cerr, ESC_SEQ_COLOR_GREEN, ESC_SEQ_COLOR_RESET);
  Loggers::develop.addStream(std::cerr, ESC_SEQ_COLOR_WHITE, ESC_SEQ_COLOR_RESET);
#endif

  ConsoleManager console;
  console.setConfig(config);

  bool ok = console.play();

  return ok ? 0 : 1;
}
