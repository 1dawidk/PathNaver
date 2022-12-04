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
    this->sentKMLSetId= 0;

    lastPNLINSend = 0;
    lastPNLISSend = 0;
}

void Talker::loop() {
    if(wsc->isConnected()) {
        if (wsc->msgsInQueue()) {
            std::string msg = wsc->popMessage();
            processIncomingMessage(msg);
        }

        if(sentKMLSetId != kmlWatcher->getKMLSetId()){
            sendPNAPN();
        }

        ulong now = tim::now();
        if (now - lastPNLINSend > TALKER_PNLIN_INTERVAL_MS) {
            sendPNLIN();
            lastPNLINSend = now;
        }

        now = tim::now();
        if (now - lastPNLISSend > TALKER_PNLIS_INTERVAL_MS) {
            sendPNLIS();
            lastPNLISSend = now;
        }
    }
}

void Talker::processIncomingMessage(const std::string &msg) {
    try {
        NMEA nmeaMsg(msg);

        if (nmeaMsg.isNamed("PNAPN")) {
            sendPNAPN();
        } else if (nmeaMsg.isNamed("PNPAD")) {
            int path_id = stoi(nmeaMsg.getValue(0));
            sendPNPAD(KML_LOCATION_EXTERNAL, path_id);
        } else if (nmeaMsg.isNamed("PNRUP")){
            int path_id = stoi(nmeaMsg.getValue(0));
            deviceConfig->setSelectedPathId(path_id);
        } else if (nmeaMsg.isNamed("PNUPL")){
            processUpload(nmeaMsg);
        }
    } catch (NMEAFormatException &e){
        Console::loge("Talker", e.what());
    }
}

void Talker::sendPNAPN() {
    NMEAData nmeaData;
    for (int i = 0; i < kmlWatcher->countFound(KML_LOCATION_EXTERNAL); i++) {
        nmeaData.add(i);
        nmeaData.add(kmlWatcher->getFileName(KML_LOCATION_EXTERNAL, i));
    }

    NMEA nmea("PNAPN", nmeaData);
    wsc->send(nmea.toString());
    Console::logd("Talker", "Sending PNAPN");
}

void Talker::sendPNPAD(int location, int id) {
    Console::logd("Talker", "PNPAD asks for "+std::to_string(id));
    NMEAData nmeaData;
    try {
        FlightPath fp = kmlWatcher->getFlightPath(location, id);
        nmeaData.add(id);
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
}

void Talker::sendPNLIN() {
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
}

void Talker::sendPNLIS() {
    NMEAData nmeaData;

    nmeaData.add(naver->getState());                    // GPS state
    nmeaData.add(deviceConfig->getSelectedPathId());    // Path id
    nmeaData.add(SuperMisc::getCpuTemp());                 // Cpu temperature

    NMEA nmea("PNLIS", nmeaData);
    wsc->send(nmea.toString());
    Console::logd("Talker", "Sending PNLIS");
}

void Talker::processUpload(NMEA &nmeaMsg) {
    std::string name = nmeaMsg.getValue(0);
    int pointsCnt= (nmeaMsg.countDataFields()-1)/2;
    std::vector<GeoPoint*> points(pointsCnt);

    for(int i=0; i<pointsCnt; i++){
        points.emplace_back(
                new GeoPoint(
                        std::stod(nmeaMsg.getValue((i*2) + 1)),
                        std::stod(nmeaMsg.getValue((i*2) + 2))));
    }

    Console::logd("Talker", "Creating kml file...");
    bool write_res= FlightPath::createKML(name, points);
    if(write_res){
        Console::logi("Talker", "New kml file \""+name+"\" created");
    } else {
        Console::loge("Talker", "Error creating kml file!");
    }

    NMEAData nmeaData;
    nmeaData.add(name);
    nmeaData.add(write_res);

    NMEA nmea("PNUPL", nmeaData);
    wsc->send(nmea.toString());
    Console::logd("Talker", "Sending PNUPL with write result");
}
