//
// Created by dkulpa on 29.07.2022.
//

#ifndef PATHNAVER_FLIGHTPATH_H
#define PATHNAVER_FLIGHTPATH_H

#include "string"
#include "rapidxml//rapidxml.hpp"
#include "fstream"
#include <memory>
#include <vector>
#include "Geo/GeoPoint.h"
#include "SuperMisc.h"
#include "Geo/GeoRoute.h"

class FlightPath {
public:
    explicit FlightPath(const std::string &path);

    void print();
    int findMyPath(const GeoPoint& p, double hdg);
    GeoRoute getRoute(int idx);
    double getDistance(int r_idx, const GeoPoint& p);
    int getShiftDirection(const GeoPoint& p, int p_idx);

    static std::string findKMLOnDrive();
private:
    static auto file2char(const std::string &path);
    static rapidxml::xml_node <>* findNodeParameter(rapidxml::xml_node <>* parent, const std::string &param_name);
    int parseFile(const std::string &path);

    std::vector<GeoPoint*> points;
    std::vector<GeoRoute*> path;
};


#endif //PATHNAVER_FLIGHTPATH_H
