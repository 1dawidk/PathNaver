//
// Created by dkulpa on 02.12.2022.
//

#ifndef PATHNAVER_KMLFILE_H
#define PATHNAVER_KMLFILE_H

#include <string>
#include "FlightPath.h"

class KMLFile {
public:
    explicit KMLFile(const std::string &path);
    ~KMLFile();
    FlightPath getFlightPath();
    std::string getFileName();

private:
    std::string filePath;
    FlightPath *flightPath
};


#endif //PATHNAVER_KMLFILE_H
