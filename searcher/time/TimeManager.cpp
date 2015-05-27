/* TimeManager.cpp
 *
 * Kubo Ryosuke
 */

#include "TimeManager.h"
#include "core/def.h"
#include "logger/Logger.h"
#include <cmath>
#include <cassert>

#define ENABLE_EASY_LOG 1

namespace {

float sigmoid(float x) {
  constexpr float g = 4.0;
  return 1.0 / (1.0 + std::exp((-g)*x));
}

} // namespace

namespace sunfish {

void TimeManager::init() {
  _depth = 0;
}

void TimeManager::nextDepth() {
  _depth++;
  assert(_depth < Tree::StackSize);
}

void TimeManager::startDepth() {
  _stack[_depth].firstMove = Move::empty();
  _stack[_depth].firstValue = -Value::Inf;
}

void TimeManager::addMove(Move move, Value value) {
  if (value > _stack[_depth].firstValue) {
    _stack[_depth].firstMove = move;
    _stack[_depth].firstValue = value;
  }
}

bool TimeManager::isEasy(float limit, float elapsed) {
  CONSTEXPR int easyDepth = 5;

  if (_depth <= easyDepth) {
    return false;
  }

  const auto& easy = _stack[_depth-easyDepth];
  const auto& prev = _stack[_depth-1];
  const auto& curr = _stack[_depth];

  limit = std::min(limit, 3600.0f);

  if (elapsed < std::max(limit * 0.02, 3.0)) {
    return false;
  }

  if (elapsed >= limit * 0.85f) {
#if ENABLE_EASY_LOG
    Loggers::message << __FILE_LINE__;
#endif
    return true;
  }

  float baseTime = std::max(limit * 0.25f, 3.0f);
  float r = 5.0f * sigmoid(0.7f * (elapsed - baseTime) / baseTime);

#if ENABLE_EASY_LOG
  {
    int easyDiff = curr.firstValue.int32() - easy.firstValue.int32();
    int prevDiff = curr.firstValue.int32() - prev.firstValue.int32();
    int isSame = curr.firstMove == easy.firstMove ? 1 : 0;
    Loggers::message << "time_manager," << r << ',' << easyDiff << ',' << prevDiff << ',' << isSame;
  }
#endif

  if (curr.firstValue >= easy.firstValue - (32 * r) && curr.firstValue <= easy.firstValue + (512 * r) &&
      curr.firstValue >= prev.firstValue - (16 * r) && curr.firstValue <= prev.firstValue + (256 * r)) {
#if ENABLE_EASY_LOG
    Loggers::message << __FILE_LINE__;
#endif
    return true;
  }

  if (curr.firstMove == easy.firstMove && curr.firstMove == prev.firstMove &&
      curr.firstValue >= prev.firstValue - (32 * r) && curr.firstValue <= prev.firstValue + (512 * r)) {
#if ENABLE_EASY_LOG
    Loggers::message << __FILE_LINE__;
#endif
    return true;
  }

  return false;
}

} // namespace sunfish
