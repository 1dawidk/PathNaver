//
// Created by dkulpa on 18.05.2022.
//

#ifndef UINS_CPP_SERIALBUFFER_H
#define UINS_CPP_SERIALBUFFER_H

#include "Serial.h"

class SerialBuffer {
public:
    SerialBuffer(const std::string& port, unsigned int baudrate, int maxlen=1024);
    std::string update();
    void close();
    bool isOpen();
private:
    Serial *serial;
    std::string buffer;
    int maxlen;
};


#endif //UINS_CPP_SERIALBUFFER_H
