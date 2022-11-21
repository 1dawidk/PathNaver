//
// Created by dkulpa on 16.11.2022.
//

#ifndef PATHNAVER_TALKER_H
#define PATHNAVER_TALKER_H


#include "TCPSerialComm.h"
#include "Worker.h"
#include "../Navigation/KMLWatcher.h"
#include "../DeviceConfig.h"
#include "../Console.h"
#include "../Navigation/Naver.h"
#include <NMEA/NMEA.h>

#define TALKER_PNLID_INTERVAL_MS   2000

class Talker : public Worker{
public:
    explicit Talker(TCPSerialComm *tsc, KMLWatcher *kmlWatcher, DeviceConfig *deviceConfig, Naver *naver);

protected:
    void loop() override;

private:
    TCPSerialComm *tsc;
    KMLWatcher *kmlWatcher;
    DeviceConfig *deviceConfig;
    Naver* naver;

    ulong lastPNLIDSend;
};


#endif //PATHNAVER_TALKER_H
