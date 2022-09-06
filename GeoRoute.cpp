//
// Created by dkulpa on 08.08.2022.
//

#include <iostream>
#include "GeoRoute.h"

GeoRoute::GeoRoute(const GeoPoint &p1, const GeoPoint &p2) : start(p2), end(p1) {
    a = (p1.getLat() - p2.getLat()) / (p1.getLon() - p2.getLon());
    b = p1.getLat() - (p1.getLon() * a);

    hdg = Geo::calcHeading(p2, p1);
    std::cout << "[INFO]\t\tRoute: a= " << a << ", b= " << b << ", hdg= " << hdg << std::endl;
    std::cout << "[INFO]\t\t\tstart: " << start.getLat() <<", " << start.getLon() << std::endl;
    std::cout << "[INFO]\t\t\tend: " << end.getLat() <<", " << end.getLon() << std::endl;
}

double GeoRoute::getHeading() const {
    return this->hdg;
}

double GeoRoute::distanceTo(const GeoPoint &p) const {
    double perp_a = -1.0 / this->a;
    double perp_b = p.getLat() - p.getLon()*perp_a;

    double clon = (perp_b-this->b) / (this->a - perp_a);
    double clat = (this->a * clon) + this->b;

    return Geo::calcDistance(p, GeoPoint(clat, clon));
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

}

double GeoRoute::distanceToEnd(const GeoPoint &p) const {
    return Geo::calcDistance(p, end);
}
