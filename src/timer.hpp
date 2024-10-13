#pragma once

#include <chrono>

namespace halloween {

    class Timer {
        public:
            std::chrono::milliseconds start;
            Timer();
            void reset();
            unsigned long int getTime();
            void setTimeRelativeToNow(unsigned long t);
    };

}
