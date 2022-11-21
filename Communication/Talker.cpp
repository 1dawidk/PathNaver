//
// Created by dkulpa on 16.11.2022.
//


#include <NMEA/NMEAData.h>
#include "Talker.h"


Talker::Talker(TCPSerialComm *tsc, KMLWatcher *kmlWatcher, DeviceConfig *deviceConfig, Naver *naver) {
    this->tsc = tsc;
    this->kmlWatcher = kmlWatcher;
    this->deviceConfig = deviceConfig;
    this->naver = naver;

    lastPNLIDSend = 0;
}

void Talker::loop() {
    if(tsc->isConnected()) {
        if (tsc->msgsInQueue()) {
            std::string msg = tsc->popMessage();
            NMEA nmeaMsg(msg);
            if (nmeaMsg.getName() == "PNAPN") {
                NMEAData nmeaData;
                nmeaData.add(kmlWatcher->countFound());
                for (int i = 0; i < kmlWatcher->countFound(); i++) {
                    nmeaData.add(i);
                    nmeaData.add(kmlWatcher->getFileName(i));
                }

                NMEA nmea("PNAPN", nmeaData);
                tsc->send(nmea.toString());
                Console::logd("Talker", "Sending PNAPN");
            } else if (nmeaMsg.getName() == "PNPAD") {
                int pathIdx = stoi(nmeaMsg.getValue(0));

                NMEAData nmeaData;
                nmeaData.add(pathIdx);

                NMEA sendNMEAMsg("PNPAD", nmeaData);
                tsc->send(sendNMEAMsg.toString());
                Console::logd("Talker", "Sending PNPAD");
            }
        }

        ulong now = tim::now();
        if (now - lastPNLIDSend > TALKER_PNLID_INTERVAL_MS) {
            NMEAData nmeaData;
            GNSSData gnssData = naver->getGNSSData();

            nmeaData.add(naver->hasFix());
            nmeaData.add(gnssData.getLat());
            nmeaData.add(gnssData.getLng());
            nmeaData.add(gnssData.getHeading());
            nmeaData.add(0);
            nmeaData.add(naver->getMyPathIdx());
            nmeaData.add(naver->getShift());

            NMEA nmea("PNLID", nmeaData);
            tsc->send(nmea.toString());
            Console::logd("Talker", "Sending PNLID");

            lastPNLIDSend = now;
        }
    }
}