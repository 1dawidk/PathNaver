//
// Created by dkulpa on 18.05.2022.
//

#ifndef UINS_CPP_SUPERSERIAL_H
#define UINS_CPP_SUPERSERIAL_H

#include "Serial.h"
#include "vector"
#include "tim.h"
#include "SuperMisc.h"

class SuperSerial {
public:
    static std::string findDevice(int baudrate,
                                  const std::string &tag,
                                  const std::string &name="serial device",
                                  bool verbose = true);

    static std::string findDevice(int baudrate,
                                  const std::vector<std::string> &tags,
                                  const std::string &name="serial device",
                                  bool verbose = true);

    static std::string findDeviceWithPoke(int baudrate,
                                          const std::string &poke,
                                          const std::string &answer,
                                          const std::string &name="serial device",
                                          bool verbose=true);

    static std::vector<std::string> listPorts();
};


#endif //UINS_CPP_SUPERSERIAL_H
