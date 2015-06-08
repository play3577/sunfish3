/* Timer.h
 *
 * Kubo Ryosuke
 */

#ifndef SUNFISH_TIMER__
#define SUNFISH_TIMER__

#include <chrono>

namespace sunfish {

class Timer {
private:

  std::chrono::time_point<std::chrono::system_clock> base_;

public:

  /**
   * set current time to base
   */
  void set() {
    base_ = std::chrono::system_clock::now();
  }

  /**
   * get current time(sec) from base
   */
  float get() const {
    auto now = std::chrono::system_clock::now();
    auto elapsed = now - base_;
    auto seconds = std::chrono::duration_cast<std::chrono::seconds>(elapsed);
    auto nanoseconds = std::chrono::duration_cast<std::chrono::nanoseconds>(elapsed);
    return seconds.count() + nanoseconds.count() * 1.0e-9f;
  }

};

} // namespace sunfish

#endif // SUNFISH_TIMER__
