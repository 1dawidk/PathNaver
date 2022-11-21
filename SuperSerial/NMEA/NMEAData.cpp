//
// Created by dkulpa on 18.11.2022.
//

#include "NMEAData.h"

#include <utility>

NMEAData::NMEAData() = default;

NMEAData::NMEAData(std::vector<std::string> d) {
    this->data= std::move(d);
}

void NMEAData::add(const std::string& v) {
    data.emplace_back(v);
}

void NMEAData::add(unsigned int v) {
    data.emplace_back(std::to_string(v));
}

void NMEAData::add(int v) {
    data.emplace_back(std::to_string(v));
}

void NMEAData::add(unsigned char v) {
    data.emplace_back(std::to_string(v));
}

void NMEAData::add(char v) {
    data.emplace_back(std::to_string(v));
}

void NMEAData::add(unsigned long v) {
    data.emplace_back(std::to_string(v));
}

void NMEAData::add(long v) {
    data.emplace_back(std::to_string(v));
}

void NMEAData::add(bool v) {
    data.emplace_back(std::to_string(v));
}

void NMEAData::add(float v) {
    data.emplace_back(std::to_string(v));
}

void NMEAData::add(double v) {
    data.emplace_back(std::to_string(v));
}

std::string NMEAData::toString() const{
    std::string s;
    for(int i=0; i<data.size(); i++){
        s+= data[i];
        if(i<(data.size()-1)){
            s+= ",";
        }
    }

    return s;
}

std::string &NMEAData::get(int i) {
    return data[i];
}

