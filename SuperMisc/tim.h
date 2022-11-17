//
// Created by dkulpa on 18.05.2022.
//

#ifndef UINS_CPP_TIM_H
#define UINS_CPP_TIM_H

#include "string"
#include <chrono>
#include <iomanip>

namespace tim{
    [[nodiscard]] unsigned long now();
    void delay(long ms);
    std::string format(unsigned long ts, bool date, bool time, bool ms);
}


#endif //UINS_CPP_TIM_H
