//
// Created by dkulpa on 06.09.2022.
//

#ifndef PATHNAVER_NAVER_H
#define PATHNAVER_NAVER_H


#include "UI/LedGUI.h"
#include "GNSSModule.h"
#include "FlightPath.h"

#define COUNTDOWN_START_DISTANCE        300.0
#define COUNTDOWN_STEP                  100.0
#define FINISH_BLINK_START_DISTANCE     40.0

class Naver {
public:
    Naver(LedGUI *gui, GNSSModule *gnssModule);
    void update();
    void loadPath(FlightPath *flightPath);
private:
    LedGUI *gui;
    GNSSModule *gnss;
    FlightPath *path;
    int blinkedFor= -1;

    unsigned long lastMsgAt;

};


#endif //PATHNAVER_NAVER_H
