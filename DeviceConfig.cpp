//
// Created by dkulpa on 16.11.2022.
//

#include "DeviceConfig.h"

DeviceConfig::DeviceConfig() {
    changeMutex.unlock();
}

void DeviceConfig::setSelectedKml(const std::string &v) {
    SET_THREAD_SAFE(changeMutex, selectedKml, v);
}

std::string DeviceConfig::getSelectedKml() {
    RETURN_THREAD_SAFE(changeMutex, std::string, selectedKml);
}
