//
// Created by dkulpa on 18.05.2022.
//
#include "tim.h"

unsigned long tim::now() {
    auto time = std::chrono::system_clock::now().time_since_epoch();
    return std::chrono::duration_cast<std::chrono::milliseconds>(time).count();
}

void tim::delay(long ms) {
    long start = tim::now();
    while((tim::now() - start) < ms);
}

std::string tim::format(unsigned long ts, bool date, bool time, bool ms){
    auto t = std::time(nullptr);
    auto tm = *std::localtime(&t);

    std::ostringstream oss;
    std::string fmt;
    if(date)
        fmt+= "%d-%m-%Y";
    if(time)
        fmt+= "%H:%M:%S";
    if(ms)
        oss << "." << std::put_time(&tm, fmt.c_str()) << (ts%1000);
    auto r = oss.str();

    return r;
}
