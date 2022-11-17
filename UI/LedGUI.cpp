//
// Created by dkulpa on 09.08.2022.
//

#include <iostream>
#include "LedGUI.h"
#include "../Console.h"


LedGUI::LedGUI(int ledsNo, int maxShift) : leds(ledsNo) {
    this->ledsNo = ledsNo;
    this->maxShift = maxShift;
    this->carouselPos = 0;
    this->carouselDir = 1;
    this->navLedIdx = 0;
    this->run = false;
    this->mode = 0;
    this->routeShift = 0;
    this->initBlinkState = 0;
    this->lastInitBlinkTime = tim::now();
    this->navBlinkFinishCnt = 0;
    this->navBlinkFinishChangeAt = 0;
    this->navCountdown = -1;
    this->sidePanelMode = SIDE_PANEL_MODE_EMPTY;
    this->run = true;
}


void LedGUI::setMode(int m) {
    vals_mutex.lock();
    if(this->mode != m) {
        if(this->mode == MODE_CAROUSEL){
            leds.clear(0);
        } else if(this->mode == MODE_INIT){
            leds.clear(0);
            leds.clear(1);
            leds.clear(2);
            leds.clear(-1);
            leds.clear(-2);
            leds.clear(-3);
        }
        leds.render();
        this->mode = m;
    }
    vals_mutex.unlock();
}

void LedGUI::stop() {
    run_mutex.lock();
    this->run = false;
    run_mutex.unlock();
}

bool LedGUI::getRunFlag() {
    run_mutex.lock();
    bool r= run;
    run_mutex.unlock();
    return r;
}

int LedGUI::getMode() {
    vals_mutex.lock();
    int m = mode;
    vals_mutex.unlock();
    return m;
}

void LedGUI::exec() {
    while(this->getRunFlag()) {
        switch (this->getMode()) {
            case LedGUI::MODE_INIT:
                initSequence();
                break;
            case LedGUI::MODE_CAROUSEL:
                carouselSequence();
                break;
            case LedGUI::MODE_NAV:
                navSequence();
                break;
            default: {
                Console::logw("LedGUI", "Unknown mode!");
                break;
            }
        }

        leds.render();

    }
}

void LedGUI::setRouteShift(double shift) {
    vals_mutex.lock();
    routeShift = shift;
    vals_mutex.unlock();
}

double LedGUI::getRouteShift() {
    vals_mutex.lock();
    double v = routeShift;
    vals_mutex.unlock();
    return v;
}

void LedGUI::carouselSequence() {
    leds.clear(carouselPos+SIDE_PANEL_SIZE);
    carouselPos+= carouselDir;
    if(carouselPos==(ledsNo-1-(2*SIDE_PANEL_SIZE)) or carouselPos==0){
        carouselDir*= -1;
    }
    leds.set(carouselPos+SIDE_PANEL_SIZE, LED_COLOR_BLUE);
}

void LedGUI::navSequence() {
    // Select nav led color
    int color = LED_COLOR_RED;
    int mainLedsNo = ledsNo- (2*SIDE_PANEL_SIZE);

    leds.set((ledsNo/2), LED_COLOR_GREEN);
    leds.clear(navLedIdx+SIDE_PANEL_SIZE);

    // Compute and set new nav led
    double a = ((1.0 - mainLedsNo) / (-2.0 * maxShift));
    double b = a * maxShift;
    this->navLedIdx = (int) round(a * getRouteShift() + b);
    if(this->navLedIdx < 0)
        this->navLedIdx = 0;
    if(this->navLedIdx>=mainLedsNo)
        this->navLedIdx = mainLedsNo-1;

    if(navLedIdx == (mainLedsNo/2)){
        color |= LED_COLOR_GREEN;
    }
    leds.set(navLedIdx+SIDE_PANEL_SIZE, color);

    run_mutex.lock();
    // Clear side panel
    for(char i=0; i<LedGUI::SIDE_PANEL_SIZE; i++){
        leds.clear(i);
        leds.clear(-1-i);
    }

    // Side panel events
    switch (this->sidePanelMode) {
        case LedGUI::SIDE_PANEL_MODE_BLINK_FINISH: {
            for (char i = 0; i < navCountdown; i++) {
                leds.set(i, LED_COLOR_GREEN);
                leds.set(-1-i, LED_COLOR_GREEN);
            }
            break;
        }
        case LedGUI::SIDE_PANEL_MODE_COUNTDOWN: {
            if(navBlinkFinishChangeAt+LedGUI::NAV_BLINK_FINISH_INTERVAL < tim::now()){
                if(navBlinkFinishCnt%2) {
                    leds.set(0, LED_COLOR_GREEN);
                    leds.set(-1, LED_COLOR_GREEN);
                }

                navBlinkFinishCnt--;
                if(!navBlinkFinishCnt)
                    sidePanelMode = LedGUI::SIDE_PANEL_MODE_EMPTY;

                navBlinkFinishChangeAt = tim::now();
            }
            break;
        }
        default:
            break;
    }
    run_mutex.unlock();
}

void LedGUI::initSequence() {
    if(lastInitBlinkTime+LedGUI::INIT_BLINK_INTERVAL < tim::now()){
        leds.set(0, LedGUI::INIT_BLINK_COLOR*initBlinkState);
        leds.set(1, LedGUI::INIT_BLINK_COLOR*initBlinkState);
        leds.set(2, LedGUI::INIT_BLINK_COLOR*initBlinkState);
        leds.set(-1, LedGUI::INIT_BLINK_COLOR*initBlinkState);
        leds.set(-2, LedGUI::INIT_BLINK_COLOR*initBlinkState);
        leds.set(-3, LedGUI::INIT_BLINK_COLOR*initBlinkState);

        initBlinkState ^= 0x00000001;
        lastInitBlinkTime = tim::now();
    }
}

void LedGUI::blinkFinish() {
    run_mutex.lock();
    navBlinkFinishCnt = 16;
    navCountdown = 0;
    sidePanelMode = SIDE_PANEL_MODE_BLINK_FINISH;
    run_mutex.unlock();
}

void LedGUI::setNavCountdown(char v) {
    run_mutex.lock();
    navCountdown= v;
    sidePanelMode = SIDE_PANEL_MODE_COUNTDOWN;
    run_mutex.unlock();
}