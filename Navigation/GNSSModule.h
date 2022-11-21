//
// Created by dkulpa on 29.07.2022.
//

#ifndef PATHNAVER_GNSSMODULE_H
#define PATHNAVER_GNSSMODULE_H

#include <SerialBuffer.h>
#include <SuperSerial.h>
#include <vector>
#include <mutex>
#include "string"
#include "Worker.h"
#include "GNSSData.h"

class GNSSModule : public Worker {
public:
    explicit GNSSModule(int baudrate, bool verbose=true, const std::string &port="");

    [[nodiscard]] bool hasFix();
    [[nodiscard]] GNSSData getData();

protected:
    void loop() override;
    void onStop() override;

private:
    bool find();
    bool startBuffer();
    void cleanup();

    bool fix;
    GNSSData gnssData;
    int baudrate;
    bool verbose;
    std::string port;
    bool fixed_port;
    SerialBuffer *serial_buffer;
    bool buffer_started;

    std::mutex dataMutex;
};


#endif //PATHNAVER_GNSSMODULE_H
