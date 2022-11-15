//
// Created by dkulpa on 08.08.2022.
//

#ifndef PATHNAVER_GEOROUTE_H
#define PATHNAVER_GEOROUTE_H

#include "GeoPoint.h"
#include "GeoCalc.h"

class GeoRoute {
public:
    GeoRoute(const GeoPoint &p1, const GeoPoint &p2);

    [[nodiscard]] double getHeading() const;
    [[nodiscard]] double distanceTo(const GeoPoint& p) const;
    [[nodiscard]] int getShiftDirection(const GeoPoint& p) const;
    [[nodiscard]] double distanceToEnd(const GeoPoint& p) const;

    [[nodiscard]] bool isOn(const GeoPoint &p) const;
private:
    GeoPoint start;
    GeoPoint end;
    double a;
    double b;
    double hdg;
};


#endif //PATHNAVER_GEOROUTE_H
