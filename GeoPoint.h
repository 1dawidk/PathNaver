//
// Created by dkulpa on 08.08.2022.
//

#ifndef PATHNAVER_GEOPOINT_H
#define PATHNAVER_GEOPOINT_H


class GeoPoint {
public:
    GeoPoint(double lat, double lng);
    GeoPoint(const GeoPoint& p);

    [[nodiscard]] double getLat() const;
    [[nodiscard]] double getLon() const;

private:
    double lat;
    double lon;
};


#endif //PATHNAVER_GEOPOINT_H
