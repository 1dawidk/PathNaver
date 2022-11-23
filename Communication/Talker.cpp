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
                    tsc->send(nmea.toString());
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
                    tsc->send(sendNMEAMsg.toString());
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
        if (now - lastPNLIDSend > TALKER_PNLID_INTERVAL_MS) {
            NMEAData nmeaData;
            GNSSData gnssData = naver->getGNSSData();

            nmeaData.add(naver->hasFix());
            nmeaData.add(gnssData.getLat());
            nmeaData.add(gnssData.getLng());
            nmeaData.add(gnssData.getHeading());
            nmeaData.add(deviceConfig->getSelectedPathId());
            nmeaData.add(naver->getMyPathIdx());
            nmeaData.add(naver->getShift());

            NMEA nmea("PNLID", nmeaData);
            tsc->send(nmea.toString());
            Console::logd("Talker", "Sending PNLID");

            lastPNLIDSend = now;
        }
    }
}