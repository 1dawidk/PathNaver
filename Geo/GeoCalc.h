//
// Created by dkulpa on 08.08.2022.
//

#ifndef PATHNAVER_GEOCALC_H
#define PATHNAVER_GEOCALC_H


#include "GeoPoint.h"
#include <cmath>

class GeoCalc {
public:
    static double calcHeading(const GeoPoint &p1, const GeoPoint &p2);
    static double calcDistance(const GeoPoint &p1, const GeoPoint &p2);
    static double calcHeadingDiff(double h1, double h2);
};


#endif //PATHNAVER_GEOCALC_H
