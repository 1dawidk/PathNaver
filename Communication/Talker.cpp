//
// Created by dkulpa on 16.11.2022.
//


#include <NMEA/NMEAData.h>
#include "Talker.h"


Talker::Talker(WirelessSerialComm *wsc, KMLWatcher *kmlWatcher, DeviceConfig *deviceConfig, Naver *naver) {
    this->wsc = wsc;
    this->kmlWatcher = kmlWatcher;
    this->deviceConfig = deviceConfig;
    this->naver = naver;

    lastPNLINSend = 0;
    lastPNLISSend = 0;
}

void Talker::loop() {
    if(wsc->isConnected()) {
        if (wsc->msgsInQueue()) {
            std::string msg = wsc->popMessage();
            try {
                NMEA nmeaMsg(msg);

                if (nmeaMsg.getName() == "PNAPN") {
                    NMEAData nmeaData;
                    nmeaData.add(kmlWatcher->countFound());
                    for (int i = 0; i < kmlWatcher->countFound(); i++) {
                        nmeaData.add(i);
                        nmeaData.add(kmlWatcher->getFileName(i));
                    }

                    NMEA nmea("PNAPN", nmeaData);
                    wsc->send(nmea.toString());
                    Console::logd("Talker", "Sending PNAPN");
                } else if (nmeaMsg.getName() == "PNPAD") {
                    int pathIdx = stoi(nmeaMsg.getValue(0));
                    Console::logd("Talker", "PNPAD asks for "+std::to_string(pathIdx));
                    NMEAData nmeaData;
                    try {
                        FlightPath fp = kmlWatcher->getFlightPath(pathIdx);
                        nmeaData.add(pathIdx);
                        nmeaData.add(fp.countPoints());
                        for(int i=0; i<fp.countPoints(); i++){
                            nmeaData.add(fp.getPoint(i)->getLat());
                            nmeaData.add(fp.getPoint(i)->getLon());
                        }
                    } catch(std::exception& e){
                        nmeaData.add(-1);
                        nmeaData.add(e.what());
                    }

                    NMEA sendNMEAMsg("PNPAD", nmeaData);
                    wsc->send(sendNMEAMsg.toString());
                    Console::logd("Talker", "Sending PNPAD");
                } else if (nmeaMsg.getName() == "PNRUP"){
                    int path_id = stoi(nmeaMsg.getValue(0));
                    deviceConfig->setSelectedPathId(path_id);
                }
            } catch (NMEAFormatException &e){
                Console::loge("Talker", e.what());
            }

        }

        ulong now = tim::now();
        if (now - lastPNLINSend > TALKER_PNLIN_INTERVAL_MS) {
            NMEAData nmeaData;
            GNSSData gnssData = naver->getGNSSData();

            nmeaData.add(naver->hasFix());
            nmeaData.add(gnssData.getLat());
            nmeaData.add(gnssData.getLng());
            nmeaData.add(gnssData.getHeading());
            nmeaData.add(deviceConfig->getSelectedPathId());
            nmeaData.add(naver->getRouteIdx());
            nmeaData.add(naver->getShift());

            NMEA nmea("PNLIN", nmeaData);
            wsc->send(nmea.toString());
            Console::logd("Talker", "Sending PNLIN");

            lastPNLINSend = now;
        }

        now = tim::now();
        if (now - lastPNLISSend > TALKER_PNLIS_INTERVAL_MS) {
            NMEAData nmeaData;

            NMEA nmea("PNLIS", nmeaData);
            wsc->send(nmea.toString());
            Console::logd("Talker", "Sending PNLIS");

            lastPNLISSend = now;
        }
    }
}