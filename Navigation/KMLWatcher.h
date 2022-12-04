//
// Created by dkulpa on 16.11.2022.
//

#ifndef PATHNAVER_KMLWATCHER_H
#define PATHNAVER_KMLWATCHER_H


#include <vector>
#include <mutex>
#include "Worker.h"
#include "string"
#include "FlightPath.h"
#include "KMLFile.h"
#include "unistd.h"
#include <random>

#define KML_LOCATION_LOCAL      0
#define KML_LOCATION_EXTERNAL   1

class KMLWatcher : public Worker{
public:
    KMLWatcher();
    int countFound(int location);
    std::string getFileName(int location, int idx);
    FlightPath getFlightPath(int location, int idx);
    [[nodiscard]] int getKMLSetId() const;

    static std::vector<KMLFile> findKMLOnDrive();
    static std::vector<KMLFile> findKMLLocal(const std::string &path);
protected:
    void onStart() override;
    void loop() override;

private:
    std::random_device rd;
    std::mt19937 mt;
    std::uniform_int_distribution<int> dist;

    std::vector<KMLFile> externalKmlsSet;
    std::vector<KMLFile> localKmlsSet;
    int kmlsSetId;
    std::mutex kmlsMutex;
};


#endif //PATHNAVER_KMLWATCHER_H
