//
// Created by dkulpa on 06.09.2022.
//

#ifndef PATHNAVER_WS281XSTRIP_H
#define PATHNAVER_WS281XSTRIP_H

#ifndef DEBUG
#include "ws2811/ws2811.h"
#endif

class WS281xStrip {
public:
    explicit WS281xStrip(int n);
    virtual ~WS281xStrip();

    void render();
    uint32_t& operator[](int i);
    void set(int i, unsigned char r, unsigned char g, unsigned char b);
    void set(int i, unsigned int c);
    void clear(int i);
private:
    int n;      //Leds number
#ifndef DEBUG
    ws2811_t wsctrlr{};
#else
    int dump;
#endif
};


#endif //PATHNAVER_WS281XSTRIP_H
