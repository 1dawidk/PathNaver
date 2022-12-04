//
// Created by dkulpa on 16.11.2022.
//

#ifndef PATHNAVER_TALKER_H
#define PATHNAVER_TALKER_H


#include "WirelessSerialComm.h"
#include "Worker.h"
#include "../Navigation/KMLWatcher.h"
#include "../DeviceConfig.h"
#include "../Console.h"
#include "../Navigation/Naver.h"
#include <NMEA/NMEA.h>

#define TALKER_PNLIN_INTERVAL_MS   2000
#define TALKER_PNLIS_INTERVAL_MS   2000

class Talker : public Worker{
public:
    explicit Talker(WirelessSerialComm *wsc, KMLWatcher *kmlWatcher, DeviceConfig *deviceConfig, Naver *naver);

protected:
    void loop() override;

private:
    void processIncomingMessage(const std::string &msg);

    void sendPNAPN();
    void sendPNPAD(int location, int id);
    void sendPNLIN();
    void sendPNLIS();
    void processUpload(NMEA &nmeaMsg);

    WirelessSerialComm *wsc;
    KMLWatcher *kmlWatcher;
    DeviceConfig *deviceConfig;
    Naver* naver;
    int sentKMLSetId;


    ulong lastPNLINSend;
    ulong lastPNLISSend;
};


#endif //PATHNAVER_TALKER_H
