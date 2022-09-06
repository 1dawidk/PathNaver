//
// Created by dkulpa on 18.05.2022.
//

#ifndef UINS_CPP_NMEA_H
#define UINS_CPP_NMEA_H

#include <utility>

#include "string"
#include "vector"
#include "SuperMisc.h"

class NMEAFormatException : public std::exception {
private:
    std::string message;

public:
    explicit NMEAFormatException(std::string msg) : message(std::move(msg)) {}
    const char * what () {
        return message.c_str();
    }
};

class NMEA {
public:
    explicit NMEA(const std::string &s);
    [[nodiscard]] std::string getName() const;
    std::string getValue(int i);

    static std::string getChecksum(const std::string &msg);
    static std::string &makeupper(std::string &s);

private:
    static void cleanupMessage(std::string &msg);
    std::string name;
    std::vector<std::string> data;
    std::string checksum;
};


#endif //UINS_CPP_NMEA_H
