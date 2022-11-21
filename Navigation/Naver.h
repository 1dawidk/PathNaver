//
// Created by dkulpa on 06.09.2022.
//

#ifndef PATHNAVER_NAVER_H
#define PATHNAVER_NAVER_H


#include "../UI/LedGUI.h"
#include "GNSSModule.h"
#include "FlightPath.h"

#define COUNTDOWN_START_DISTANCE        300.0
#define COUNTDOWN_STEP                  100.0
#define FINISH_BLINK_START_DISTANCE     40.0

class Naver : public Worker {
public:
    Naver(LedGUI *gui, GNSSModule *gnssModule);
    void loadPath(FlightPath *flightPath);

    [[nodiscard]] bool hasFix() const;
    [[nodiscard]] GNSSData getGNSSData() const;

    void pause();
    void resume();

    double getShift() const;
    int getMyPathIdx() const;

protected:
    void loop() override;

private:
    bool paused;

    LedGUI *gui;
    GNSSModule *gnss;
    FlightPath *path;
    int blinkedFor= -1;

    unsigned long lastMsgAt;

    double shift;
    int myPathIdx;

    std::mutex pathMutex;
};


#endif //PATHNAVER_NAVER_H
