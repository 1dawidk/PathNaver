//
// Created by dkulpa on 20.05.2022.
//

#include "SuperMisc.h"

std::vector<std::string> SuperMisc::split(const std::string &s, char d) {
    size_t start = 0;
    size_t end;
    std::vector<std::string> parts;

    while ((end = s.find(d, start)) != std::string::npos) {
        std::string next= std::string(s.substr(start, (end-start)));
        if(!next.empty())
            parts.emplace_back(next);
        start = end+1;

    }
    if(start<(s.length()-1))
        parts.emplace_back(s.substr(start));

    return parts;
}

std::string SuperMisc::exec(const std::string &cmd) {
    std::array<char, 128> buffer{};
    std::string result;
    std::unique_ptr<FILE, decltype(&pclose)> pipe(popen(cmd.c_str(), "r"), pclose);
    if (!pipe) {
        throw std::runtime_error("popen() failed!");
    }
    while (fgets(buffer.data(), buffer.size(), pipe.get()) != nullptr) {
        result += buffer.data();
    }

    return result;
}

void SuperMisc::removeChar(std::string &s, char c) {
    size_t pos;
    while((pos=s.find(c)) != std::string::npos){
        s.erase(pos,1);
    }
}