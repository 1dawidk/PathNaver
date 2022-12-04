//
// Created by dkulpa on 06.09.2022.
//

#include <iostream>
#include "WS281xStrip.h"

WS281xStrip::WS281xStrip(int n) {
    this->n = n;
#ifndef DEBUG
    this->wsctrlr={
            .freq = WS2811_TARGET_FREQ,
            .dmanum = 10,
            .channel = {
                    {   .gpionum = 18,
                            .invert = 0,
                            .count = n,
                            .strip_type = WS2811_STRIP_GBR,
                            .brightness = 255,
                    }, {
                            .gpionum = 0,
                            .invert = 0,
                            .count = 0,
                            .brightness = 0,
                    },
            },
    };
    ws2811_init(&wsctrlr);
#endif
}

uint32_t& WS281xStrip::operator[](int i) {
#ifndef DEBUG
    if(i > n-1)
        i = n-1;

    if(i < -n)
        i = -n;

    if(i >= 0) {
        return wsctrlr.channel[0].leds[i];
    } else {
        return wsctrlr.channel[0].leds[n + i];
    }
#else
    return reinterpret_cast<uint32_t &>(this->dump);
#endif
}

void WS281xStrip::render() {
#ifndef DEBUG
    ws2811_render(&wsctrlr);
#endif
}

void WS281xStrip::set(const int i, const unsigned char r, const unsigned char g, const unsigned char b) {
    (*this)[i]= 0x00000000 | r | (g << 8) | (b << 16);
}

void WS281xStrip::set(const int i, const unsigned int c) {
    (*this)[i]= c;
}

WS281xStrip::~WS281xStrip() {
#ifndef DEBUG
    ws2811_fini(&wsctrlr);
#endif
}

void WS281xStrip::clear(int i) {
    (*this)[i]= 0x00000000;
}
