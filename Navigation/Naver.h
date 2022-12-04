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
    explicit Naver(GNSSModule *gnssModule);
    void loadPath(FlightPath *flightPath, int id);
    bool isPathLoaded();

    [[nodiscard]] bool hasFix() const;
    [[nodiscard]] int getState() const;
    [[nodiscard]] GNSSData getGNSSData() const;

    void pause();
    void resume();

    [[nodiscard]] double getShift() const;
    [[nodiscard]] int getPathId() const;
    [[nodiscard]] int getRouteIdx() const;
    [[nodiscard]] int getNavCountdown() const;
    bool getPassedEndFlag();

protected:
    void loop() override;

private:
    bool paused;

    GNSSModule *gnss;
    int pathId;
    FlightPath *path;
    int blinkedFor= -1;

    unsigned long lastMsgAt;

    double shift;
    int myPathIdx;
    int navCountdown=0;
    bool passedEndFlag=false;

    std::mutex pathMutex;
};


#endif //PATHNAVER_NAVER_H
