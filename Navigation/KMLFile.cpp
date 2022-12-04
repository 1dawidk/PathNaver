//
// Created by dkulpa on 02.12.2022.
//

#include "KMLFile.h"

KMLFile::KMLFile(const std::string &path) {
    filePath= path;
    flightPath= nullptr;
}

FlightPath KMLFile::getFlightPath() {
    if(flightPath== nullptr){
        flightPath= new FlightPath(filePath);
    }

    return *flightPath;
}

KMLFile::~KMLFile() {
    delete flightPath;
}

std::string KMLFile::getFileName() {
    return std::string("test_getFileName");
}
