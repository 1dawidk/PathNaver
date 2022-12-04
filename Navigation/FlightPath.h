//
// Created by dkulpa on 29.07.2022.
//

#ifndef PATHNAVER_FLIGHTPATH_H
#define PATHNAVER_FLIGHTPATH_H

#define KML_XML_HEADER "<?xml version=\"1.0\" encoding=\"UTF-8\"?>"
#define KML_HEADER  "<kml xmlns=\"http://www.opengis.net/kml/2.2\" xmlns:gx=\"http://www.google.com/kml/ext/2.2\" xmlns:kml=\"http://www.opengis.net/kml/2.2\" xmlns:atom=\"http://www.w3.org/2005/Atom\">"

#include "string"
#include "../rapidxml/rapidxml.hpp"
#include "fstream"
#include <memory>
#include <vector>
#include "../Geo/GeoPoint.h"
#include "SuperMisc.h"
#include "../Geo/GeoRoute.h"

class FlightPath {
public:
    explicit FlightPath(const std::string &path);

    [[maybe_unused]] void print();
    int findMyPath(const GeoPoint& p, double hdg);
    GeoRoute getRoute(int idx);
    [[maybe_unused]] double getDistance(int r_idx, const GeoPoint& p);
    [[maybe_unused]] int getShiftDirection(const GeoPoint& p, int p_idx);

    [[maybe_unused]] static bool createKML(const std::string &name, const std::vector<GeoPoint*> &ps);

    int countPoints();
    GeoPoint* getPoint(int idx);
private:
    static auto file2char(const std::string &path);
    static rapidxml::xml_node <>* findNodeParameter(rapidxml::xml_node <>* parent, const std::string &param_name);
    int parseFile(const std::string &path);

    std::string name;
    std::vector<GeoPoint*> points;
    std::vector<GeoRoute*> path;
};


#endif //PATHNAVER_FLIGHTPATH_H
