//
// Created by dkulpa on 18.05.2022.
//

#ifndef UINS_CPP_SERIAL_H
#define UINS_CPP_SERIAL_H

#include "string"
#include <iostream>
#include <cstdio>      // standard input / output functions
#include <cstdlib>
#include <cstring>     // string function definitions
#include <unistd.h>     // UNIX standard function definitions
#include <fcntl.h>      // File control definitions
#include <termios.h>    // POSIX terminal control definitions

#define SERIAL_BAUDRATE(x) B##x


class Serial {
public:
    Serial(const std::string &port, unsigned int baudrate);
    virtual ~Serial();

    int start();
    void stop();

    void transmit(const std::string &msg) const;
    [[nodiscard]] std::string receive() const;

    [[nodiscard]] bool isOpen() const;

private:
    [[nodiscard]] int configPort() const;
    static unsigned int translate_baudrate(unsigned int raw);

    int fd;
    std::string port;
    unsigned int baudrate;
};




#endif //UINS_CPP_SERIAL_H
