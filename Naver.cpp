//
// Created by dkulpa on 06.09.2022.
//

#include "Naver.h"
#include "Geo/GeoPoint.h"
#include "Geo/GeoRoute.h"

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

            if(lastMsgAt+1000 < tim::now()) {
                std::cout << "My route: " << my_path_idx << ", offset: " << my_path.distanceTo(me) << "m, "
                          << my_path.distanceToEnd(me)
                          << "m left" << std::endl;
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
