//
// Created by dkulpa on 29.07.2022.
//

#ifndef PATHNAVER_GNSSMODULE_H
#define PATHNAVER_GNSSMODULE_H

#include <SerialBuffer.h>
#include <SuperSerial.h>
#include <vector>
#include "string"


class GNSSModule {
public:
    explicit GNSSModule(int baudrate, bool verbose=true, const std::string &port="");
    bool find();
    void startBuffer();
    void update();
    void cleanup();

    [[nodiscard]] bool isOpen() const;
    [[nodiscard]] double getLat() const;
    [[nodiscard]] double getLng() const;
    [[nodiscard]] double getHeading() const;
    [[nodiscard]] bool hasFix() const;

private:
    bool fix;
    double lat;
    double lng;
    double heading;
    int baudrate;
    bool verbose;
    std::string port;
    bool fixed_port;
    SerialBuffer *serial_buffer;
    bool buffer_started;
};


#endif //PATHNAVER_GNSSMODULE_H
