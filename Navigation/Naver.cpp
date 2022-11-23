//
// Created by dkulpa on 06.09.2022.
//

#include "Naver.h"
#include "../Console.h"

Naver::Naver(LedGUI *ledgui, GNSSModule *gnssModule) {
    this->gui = ledgui;
    this->gnss = gnssModule;
    this->path = nullptr;
    lastMsgAt = 0;
    this->paused = true;
    this->pathMutex.unlock();

    shift=0;
    myPathIdx=0;
}

void Naver::loadPath(FlightPath *flightPath) {
    pathMutex.lock();
    path = flightPath;
    pathMutex.unlock();
}

void Naver::loop() {
    if(!paused && (path != nullptr)) {
        if (gnss->hasFix()) {
            gui->setMode(LedGUI::MODE_NAV);
            GNSSData gnssData = gnss->getData();

            pathMutex.lock();
            GeoPoint me(gnssData.getLat(), gnssData.getLng());
            myPathIdx = path->findMyPath(me, gnssData.getHeading());
            GeoRoute my_path = path->getRoute(myPathIdx);
            shift = my_path.distanceTo(me) * (double) my_path.getShiftDirection(me);
            gui->setRouteShift(shift);

            if (my_path.distanceToEnd(me) < COUNTDOWN_START_DISTANCE) {
                if (blinkedFor != myPathIdx) {
                    if (my_path.distanceToEnd(me) < FINISH_BLINK_START_DISTANCE) {
                        gui->blinkFinish();
                        blinkedFor = myPathIdx;
                    } else {
                        gui->setNavCountdown((char) ((my_path.distanceToEnd(me) / COUNTDOWN_STEP) + 1));
                    }
                }
            }
            pathMutex.unlock();

            // Logging
            if (lastMsgAt + 1000 < tim::now()) {
                Console::logi("KML", "My route: " + std::to_string(myPathIdx) + ", offset: " +
                                     std::to_string(my_path.distanceTo(me)) +
                                     "m, " + std::to_string(my_path.distanceToEnd(me)) + "m left");
                lastMsgAt = tim::now();
            }
        } else {
            gui->setMode(LedGUI::MODE_CAROUSEL);
        }
        Worker::sleep(100);
    } else {
        gui->setMode(LedGUI::MODE_CAROUSEL);
        Worker::sleep(500);
    }
}

void Naver::pause() {
    paused = true;
}

void Naver::resume() {
    paused = false;
}

GNSSData Naver::getGNSSData() const {
    return gnss->getData();
}

bool Naver::hasFix() const {
    return gnss->hasFix();
}

double Naver::getShift() const {
    return shift;
}

int Naver::getMyPathIdx() const {
    return myPathIdx;
}
