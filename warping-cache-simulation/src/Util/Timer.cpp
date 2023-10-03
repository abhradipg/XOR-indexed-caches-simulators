#include "Timer.hpp"

Timer::Timer(const Timer::Event &event) {
  this->event = event;
  this->startTime = std::chrono::system_clock::now();
}

Timer::TimerResult Timer::End() {
  auto endTime = std::chrono::system_clock::now();
  assert(!this->event.empty() && "Event exists");
  auto duration = endTime - this->startTime;

  return {
      this->event,
      std::chrono::duration_cast<std::chrono::milliseconds>(duration).count(),
      std::chrono::duration_cast<std::chrono::seconds>(duration).count(),
      std::chrono::duration_cast<std::chrono::minutes>(duration).count()};
}

void Timer::TimerResult::Print() const {
  std::cout << event << " took " << inMilliseconds << " ms"
            << " = " << inSeconds << "s"
            << " = " << inMinutes << "m" << std::endl;
}
