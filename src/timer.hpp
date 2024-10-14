#pragma once

#include <chrono>

namespace halloween {

    class Timer {
        public:
            std::chrono::milliseconds start;
            Timer();
            void reset();

            // Get time difference between now and the start time in milliseconds
            unsigned long int getTime();

            // Set the start time such that the difference between now and the start time
            // is `t` milliseconds.
            void setTime(unsigned long t);
    };

}
