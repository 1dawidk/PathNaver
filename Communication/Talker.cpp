//
// Created by dkulpa on 16.11.2022.
//

#include <NMEA/NMEA.h>
#include "Talker.h"
#include "../Console.h"

Talker::Talker(TCPSerialComm *tsc, KMLWatcher *kmlWatcher, DeviceConfig *deviceConfig) {
    this->tsc = tsc;
    this->kmlWatcher = kmlWatcher;
    this->deviceConfig = deviceConfig;
}

void Talker::loop() {
    if(tsc->msgsInQueue()){
        std::string msg = tsc->popMessage();
        if(msg.find("PNAPN")!=std::string::npos){
            std::vector<std::string> data;
            data.emplace_back(std::to_string(kmlWatcher->countFound()));
            for(int i=0; i<kmlWatcher->countFound(); i++){
                data.emplace_back(std::to_string(i));
                data.emplace_back(kmlWatcher->getFileName(i));
            }
            NMEA nmea("PNAPN", data);
            tsc->send(nmea.toString());
            Console::logd("main", "Sending "+nmea.toString());
        }
    }
}