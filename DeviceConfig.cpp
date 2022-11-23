//
// Created by dkulpa on 16.11.2022.
//

#include "DeviceConfig.h"

DeviceConfig::DeviceConfig() {
    selectedPathId= 0;
    changeMutex.unlock();
    stateChanged= true;
}

void DeviceConfig::setSelectedPathId(int v) {
    SET_THREAD_SAFE(changeMutex, stateChanged, true);
    SET_THREAD_SAFE(changeMutex, selectedPathId, v);
}

int DeviceConfig::getSelectedPathId() {
    RETURN_THREAD_SAFE(changeMutex, int, selectedPathId);
}

bool DeviceConfig::hasChanged() {
    changeMutex.lock();
    bool v= stateChanged;
    stateChanged= false;
    changeMutex.unlock();

    return v;
}
