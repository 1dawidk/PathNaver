//
// Created by dkulpa on 16.11.2022.
//

#include "DeviceConfig.h"

DeviceConfig::DeviceConfig() {
    selectedPathId= 0;
    changeMutex.unlock();
}

void DeviceConfig::setSelectedPathId(int v) {
    SET_THREAD_SAFE(changeMutex, selectedPathId, v);
}

int DeviceConfig::getSelectedPathId() {
    RETURN_THREAD_SAFE(changeMutex, int, selectedPathId);
}
