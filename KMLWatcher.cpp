//
// Created by dkulpa on 16.11.2022.
//

#include "KMLWatcher.h"
#include "FlightPath.h"

void KMLWatcher::onStart() {
    kmlsMutex.unlock();
}

void KMLWatcher::loop() {
    kmlsMutex.lock();
    kmls= FlightPath::findKMLOnDrive();
    kmlsMutex.unlock();

    std::this_thread::sleep_for(std::chrono::microseconds(1000*1000*2));
}

int KMLWatcher::countFound() {
    kmlsMutex.lock();
    int c = static_cast<int>(kmls.size());
    kmlsMutex.unlock();

    return c;
}

std::string KMLWatcher::getFileName(int idx) {
    kmlsMutex.lock();
    std::string s = kmls[idx];
    kmlsMutex.unlock();

    return s;
}


