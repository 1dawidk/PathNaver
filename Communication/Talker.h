//
// Created by dkulpa on 16.11.2022.
//

#ifndef PATHNAVER_TALKER_H
#define PATHNAVER_TALKER_H


#include "TCPSerialComm.h"
#include "../Worker.h"
#include "../KMLWatcher.h"
#include "../DeviceConfig.h"

class Talker : public Worker{
public:
    explicit Talker(TCPSerialComm *tsc, KMLWatcher *kmlWatcher, DeviceConfig *deviceConfig);

protected:
    void loop() override;

private:
    TCPSerialComm *tsc;
    KMLWatcher *kmlWatcher;
    DeviceConfig *deviceConfig;
};


#endif //PATHNAVER_TALKER_H
