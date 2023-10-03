#pragma once

#include <cassert>
#include <chrono>
#include <iostream>
#include <unordered_map>
#include <vector>

class Timer {
public:
  typedef std::string Event;
  typedef std::chrono::time_point<std::chrono::system_clock> TimePoint;

  struct TimerResult {
    Event event;
    long inMilliseconds;
    long inSeconds;
    long inMinutes;

    void Print() const;
  };

  explicit Timer(const Event &event);
  TimerResult End();

private:
  Event event = "";
  TimePoint startTime;
};
