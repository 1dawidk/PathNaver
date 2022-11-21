//
// Created by dkulpa on 18.11.2022.
//

#include "GNSSData.h"

GNSSData::GNSSData() {
    lat = 0;
    lng = 0;
    heading = 0;
}

double GNSSData::getLat() const {
    return lat;
}

void GNSSData::setLat(double v) {
    this->lat = v;
}

double GNSSData::getLng() const {
    return lng;
}

void GNSSData::setLng(double v) {
    this->lng = v;
}

double GNSSData::getHeading() const {
    return heading;
}

void GNSSData::setHeading(double v) {
    this->heading = v;
}
