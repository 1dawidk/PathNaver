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

#define GNSS_MODULE_NOT_FOUND   2
#define GNSS_MODULE_NO_FIX      1
#define GNSS_MODULE_OK          0

class GNSSModule : public Worker {
public:
    explicit GNSSModule(int baudrate, bool verbose=true, const std::string &port="");

    [[nodiscard]] bool hasFix();
    [[nodiscard]] int getState() const;
    [[nodiscard]] GNSSData getData();

protected:
    void loop() override;
    void onStop() override;

private:
    bool find();
    bool startBuffer();
    void cleanup();

    GNSSData gnssData;
    int baudrate;
    bool verbose;
    std::string port;
    bool fixed_port;
    SerialBuffer *serial_buffer;
    bool buffer_started;

    char state;

    std::mutex dataMutex;
};


#endif //PATHNAVER_GNSSMODULE_H
