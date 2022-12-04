//
// Created by dkulpa on 08.08.2022.
//

#include <iostream>
#include "GeoRoute.h"
#include "../Console.h"

GeoRoute::GeoRoute(const GeoPoint &p1, const GeoPoint &p2) : start(p2), end(p1) {
    a = (p1.getLat() - p2.getLat()) / (p1.getLon() - p2.getLon());
    b = p1.getLat() - (p1.getLon() * a);

    hdg = GeoCalc::calcHeading(p2, p1);
    //Console::logi("GeoRoute", "Route: a= "+std::to_string(a)+", b= "+std::to_string(b)+", hdg= "+std::to_string(hdg));
    //Console::logi("GeoRoute", "\tstart: "+std::to_string(start.getLat())+", "+std::to_string(start.getLon()));
    //Console::logi("GeoRoute", "\tend: "+std::to_string(end.getLat())+", "+std::to_string(end.getLon()));
}

double GeoRoute::getHeading() const {
    return this->hdg;
}

double GeoRoute::distanceTo(const GeoPoint &p) const {
    double perp_a = -1.0 / this->a;
    double perp_b = p.getLat() - p.getLon()*perp_a;

    double clon = (perp_b-this->b) / (this->a - perp_a);
    double clat = (this->a * clon) + this->b;

    return GeoCalc::calcDistance(p, GeoPoint(clat, clon));
}

int GeoRoute::getShiftDirection(const GeoPoint &p) const {
    if(p.getLat() < (a*p.getLon() + b)){
        if((hdg>=0) and (hdg<=180)){
            return 1;
        } else {
            return -1;
        }
    } else {
        if((hdg>=0) and (hdg<=180)){
            return -1;
        } else {
            return 1;
        }
    }
}

bool GeoRoute::isOn(const GeoPoint &p) const {
    double ap= -1/a;
    double bp;

    return false;
}

double GeoRoute::distanceToEnd(const GeoPoint &p) const {
    return GeoCalc::calcDistance(p, end);
}
