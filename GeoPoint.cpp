//
// Created by dkulpa on 08.08.2022.
//

#include "GeoPoint.h"

GeoPoint::GeoPoint(double lat, double lng)  {
    this->lat = lat;
    this->lon = lng;
}

double GeoPoint::getLat() const {
    return lat;
}

double GeoPoint::getLon() const {
    return lon;
}

GeoPoint::GeoPoint(const GeoPoint &p) {
    this->lat= p.lat;
    this->lon= p.lon;
}


