#include "timer.hpp"

namespace halloween {

    Timer::Timer() { reset(); }
    void Timer::reset() {
        start = duration_cast<std::chrono::milliseconds>(
                std::chrono::system_clock::now().time_since_epoch());
    }
    unsigned long int Timer::getTime() {
        auto current_time = duration_cast<std::chrono::milliseconds>(
                std::chrono::system_clock::now().time_since_epoch());
        return (current_time - start).count();
    }
}
