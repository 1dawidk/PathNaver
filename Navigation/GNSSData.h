//
// Created by dkulpa on 18.11.2022.
//

#ifndef PATHNAVER_GNSSDATA_H
#define PATHNAVER_GNSSDATA_H


class GNSSData {
public:
    GNSSData();

    [[maybe_unused]] [[nodiscard]] double getLat() const;
    [[maybe_unused]] void setLat(double lat);

    [[maybe_unused]] [[nodiscard]] double getLng() const;
    [[maybe_unused]] void setLng(double lng);

    [[maybe_unused]] [[nodiscard]] double getHeading() const;
    [[maybe_unused]] void setHeading(double heading);

private:
    double lat;
    double lng;
    double heading;
};


#endif //PATHNAVER_GNSSDATA_H
