//
// Created by dkulpa on 09.08.2022.
//

#ifndef PATHNAVER_LEDGUI_H
#define PATHNAVER_LEDGUI_H

#include <cmath>
#include <thread>
#include <mutex>
#include <chrono>
#include "tim.h"
#include "WS281xStrip.h"

#define LED_CLEAR       0
#define LED_COLOR_BLUE  0x00ff0000
#define LED_COLOR_RED   0x000000ff
#define LED_COLOR_GREEN 0x0000ff00

class LedGUI {
public:
    LedGUI(int ledsNo, int maxShift); // Max shift in meters

    // Run mechanism
    void exec();
    void stop();

    // Getters, setters
    void setMode(int m);
    void setRouteShift(double shift);
    double getRouteShift();
    bool getRunFlag();
    int getMode();
    void blinkFinish();
    void setNavCountdown(char v);


    // Parameters
    static const int MODE_INIT = 0;
    static const int MODE_CAROUSEL = 1;
    static const int MODE_NAV = 2;

private:
    static const char SIDE_PANEL_SIZE = 3;
    std::mutex run_mutex;
    bool run;
    std::mutex vals_mutex;
    int mode;
    double routeShift;
    int ledsNo;
    int maxShift;

    int sidePanelMode;
    static const int SIDE_PANEL_MODE_EMPTY = 0;
    static const int SIDE_PANEL_MODE_BLINK_FINISH = 1;
    static const int SIDE_PANEL_MODE_COUNTDOWN = 2;

    // Carousel sequence
    void carouselSequence();
    int carouselPos;
    int carouselDir;

    // Navigation sequence
    void navSequence();
    int navLedIdx;
    char navCountdown;
    char navBlinkFinishCnt= 0;
    unsigned long navBlinkFinishChangeAt;
    const static long NAV_BLINK_FINISH_INTERVAL = 150; // ms

    // Initialization sequence
    unsigned long lastInitBlinkTime;
    int initBlinkState;
    static const unsigned long INIT_BLINK_INTERVAL = 100; // ms
    static const int INIT_BLINK_COLOR = 0x00111111; // WWBBGGRR
    void initSequence();

    WS281xStrip leds;
};


#endif //PATHNAVER_LEDGUI_H
