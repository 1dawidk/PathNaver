//
// Created by dkulpa on 29.07.2022.
//

#include <iostream>
#include "FlightPath.h"

auto FlightPath::file2char( const std::string &fil_path ) {
    std::ifstream file(fil_path);
    if( !file ) {
        throw std::runtime_error( "[ERR] File '" + fil_path + " can't be opened!" );
    }
    file.seekg( 0, std::ifstream::end );
    const auto fileSize = file.tellg();
    file.seekg( 0 );

    std::unique_ptr < char[] > fileContext( new char[ fileSize + static_cast < decltype( fileSize ) >( 1 ) ] );
    file.read( fileContext.get(), fileSize );

    return fileContext;
}

rapidxml::xml_node<> *FlightPath::findNodeParameter(rapidxml::xml_node<> *parent, const std::string &param_name) {
    rapidxml::xml_node <>* child;
    rapidxml::xml_node <>* found = nullptr;
    for(child = parent->first_node(); child; child = child->next_sibling()){
        if(std::string(child->name()) == param_name){
            found = child;
            break;
        }
    }

    return found;
}

FlightPath::FlightPath(const std::string &fil_path) {
    if(parseFile(fil_path))
        throw std::runtime_error("Cannot parse KML file!");
}

int FlightPath::parseFile(const std::string &fil_path) {
    rapidxml::xml_document <> kml;
    auto text = FlightPath::file2char(fil_path);

    try{
        kml.parse <0>( text.get() ); // 0 to domyślny tryb parsowania
    } catch( const rapidxml::parse_error & e ) {
        std::cerr << e.what() << " here: " << e.where < char >() << std::endl;
        return - 1;
    }

    // Find Document node
    rapidxml::xml_node <>* docNode = kml.first_node();
    if(std::string(docNode->name()) == "kml"){
        docNode = docNode->first_node();
    } else {
        std::cerr << "[ERR] Incorrect KML format!" << std::endl;
        return - 2;
    }

    while (std::string(docNode->name()) != "Document"){
        docNode = docNode->next_sibling();
    }

    rapidxml::xml_node <>* n;
    for(n = docNode->first_node(); n; n = n->next_sibling()) {
        std::cout << "[INFO] Document sub node: " <<  n->name() << std::endl;

        if( std::string(n->name()) == "Placemark" ) {
            rapidxml::xml_node <>* lineStringNode= FlightPath::findNodeParameter(n, "LineString");
            if(lineStringNode != nullptr){
                rapidxml::xml_node <>* coordsNode=
                        FlightPath::findNodeParameter(lineStringNode, "coordinates");

                if(coordsNode != nullptr) {
                    std::string val = coordsNode->value();
                    std::vector<std::string> ps = SuperMisc::split(val, ' '); // Strings with point params
                    std::cout << "[INFO]\tFound line with " << ps.size() << " points" << std::endl;
                    for (auto &p: ps) {

                        SuperMisc::removeChar(p, '\n');
                        SuperMisc::removeChar(p, '\t');
                        if (!p.empty()) {
                            auto vals = SuperMisc::split(p, ',');
                            double lat, lon;
                            lat = std::stod(vals[1]);
                            lon = std::stod(vals[0]);
                            std::cout.precision(12);
                            std::cout << "[INFO]\t\tParsed point: " << lat << ", " << lon << " [lat/lon]" << std::endl;
                            this->points.emplace_back(new GeoPoint(lat, lon));

                            if(this->points.size() > 1){
                                this->path.emplace_back(new GeoRoute(*this->points[this->points.size()-1],
                                        *this->points[this->points.size()-2]));
                            }
                        }
                    }
                }
            }
        } else {
            std::cout << "[INFO]\tInsignificant..." << std::endl;
        }
    }


    return 0;
}

void FlightPath::print() {
    std::cout << "[INFO] Flight Route (" << points.size() << " points)" << std::endl;
    for(const auto& p: this->points){
        std::cout << "[INFO]\t" << p->getLat() << " / " << p->getLon() << std::endl;
    }
}

int FlightPath::findMyPath(const GeoPoint& p, double hdg) {
    int idx = 0;
    int nearestIdx=0;
    double nearestDist= fabs(path[0]->distanceTo(p));

    for(const auto& r: path){
        if (Geo::calcHeadingDiff(r->getHeading(), hdg) < 45){
            double dist = fabs(r->distanceTo(p));
            if (dist < nearestDist){
                nearestIdx = idx;
                nearestDist = dist;
            }
        }

        idx++;
    }

    return nearestIdx;
}

double FlightPath::getDistance(int r_idx, const GeoPoint& p) {
    return path[r_idx]->distanceTo(p);
}

int FlightPath::getShiftDirection(const GeoPoint &p, int p_idx) {
    return path[p_idx]->getShiftDirection(p);
}

std::string FlightPath::findKMLOnDrive() {
    std::string r= SuperMisc::exec("find /media/pi/ -name '*.kml'");

    if((r.find("find")==std::string::npos) and !r.empty()) {
        std::vector<std::string> lines = SuperMisc::split(r, '\n');
        if(!lines.empty())
            return lines[0];
        else
            return "";
    } else {
        return "";
    }
}

GeoRoute FlightPath::getRoute(int idx) {
    return GeoRoute(*path[idx]);
}
