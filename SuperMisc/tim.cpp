//
// Created by dkulpa on 18.05.2022.
//

#include <chrono>
#include "tim.h"

unsigned long tim::now() {
    auto time = std::chrono::system_clock::now().time_since_epoch();
    return std::chrono::duration_cast<std::chrono::milliseconds>(time).count();
}

void tim::delay(long ms) {
    long start = tim::now();
    while((tim::now() - start) < ms);
}
