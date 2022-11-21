//
// Created by dkulpa on 20.05.2022.
//

#ifndef UINS_CPP_SUPERMISC_H
#define UINS_CPP_SUPERMISC_H

#include "vector"
#include "string"

#include <iostream>
#include <memory>
#include <stdexcept>
#include <string>
#include <array>

namespace SuperMisc{
    std::vector<std::string> split(const std::string &s, char d);
    std::string exec(const std::string &cmd);
    void removeChar(std::string &s, char c);
}

#endif //UINS_CPP_SUPERMISC_H
