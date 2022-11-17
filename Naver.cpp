//
// Created by dkulpa on 06.09.2022.
//

#include "Naver.h"
#include "Geo/GeoPoint.h"
#include "Geo/GeoRoute.h"
#include "Console.h"

Naver::Naver(LedGUI *ledgui, GNSSModule *gnssModule) {
    this->gui = ledgui;
    this->gnss = gnssModule;
    this->path = nullptr;
    lastMsgAt = 0;
}

void Naver::update() {
    if(!gnss->isOpen()){
        gui->setMode(LedGUI::MODE_CAROUSEL);
        gnss->cleanup();
        gnss->startBuffer();
    } else {
        gnss->update();
        if (gnss->hasFix()) {
            gui->setMode(LedGUI::MODE_NAV);
            GeoPoint me(gnss->getLat(), gnss->getLng());
            int my_path_idx = path->findMyPath(me, gnss->getHeading());
            GeoRoute my_path = path->getRoute(my_path_idx);
            double shift = my_path.distanceTo(me) * (double)my_path.getShiftDirection(me);
            gui->setRouteShift(shift);

            if(my_path.distanceToEnd(me) < COUNTDOWN_START_DISTANCE) {
                if(blinkedFor != my_path_idx) {
                    if (my_path.distanceToEnd(me) < FINISH_BLINK_START_DISTANCE) {
                        gui->blinkFinish();
                        blinkedFor = my_path_idx;
                    } else {
                        gui->setNavCountdown((char)((my_path.distanceToEnd(me)/COUNTDOWN_STEP)+1));
                    }
                }
            }

            // Logging
            if(lastMsgAt+1000 < tim::now()) {
                Console::logi("KML", "My route: "+std::to_string(my_path_idx)+", offset: "+std::to_string(my_path.distanceTo(me))+
                                    "m, "+std::to_string(my_path.distanceToEnd(me))+"m left");
                lastMsgAt = tim::now();
            }
        } else {
            gui->setMode(LedGUI::MODE_CAROUSEL);
        }
    }
}

void Naver::loadPath(FlightPath *flightPath) {
    this->path = flightPath;
}
