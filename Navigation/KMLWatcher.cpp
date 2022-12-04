//
// Created by dkulpa on 16.11.2022.
//

#include "KMLWatcher.h"
#include "FlightPath.h"

KMLWatcher::KMLWatcher() : mt(rd()), dist(1, INT32_MAX) {
    kmlsSetId= 0;
}

void KMLWatcher::onStart() {
    kmlsMutex.unlock();
}

void KMLWatcher::loop() {
    kmlsMutex.lock();
    externalKmlsSet= KMLWatcher::findKMLOnDrive();
    localKmlsSet= KMLWatcher::findKMLLocal("./");
    kmlsSetId= dist(mt);
    kmlsMutex.unlock();

    std::this_thread::sleep_for(std::chrono::microseconds(1000*1000*2));
}

int KMLWatcher::countFound(int location) {
    kmlsMutex.lock();
    int c;
    if(location == KML_LOCATION_LOCAL)
        c = static_cast<int>(localKmlsSet.size());
    else
        c=  static_cast<int>(externalKmlsSet.size());
    kmlsMutex.unlock();

    return c;
}

std::string KMLWatcher::getFileName(int location, int idx) {
    kmlsMutex.lock();
    std::string path;
    if(location==KML_LOCATION_LOCAL)
        path = localKmlsSet[idx].getFileName();
    else
        path = externalKmlsSet[idx].getFileName();
    kmlsMutex.unlock();

    size_t start = path.rfind('/');
    size_t end = path.rfind('.');
    std::string s = path.substr(start+1, end-start-1);

    return s;
}

FlightPath KMLWatcher::getFlightPath(int location, int idx) {
    kmlsMutex.lock();
    std::string path;
    if(location==KML_LOCATION_LOCAL)
        path = localKmlsSet[idx].getFileName();
    else
        path = externalKmlsSet[idx].getFileName();
    FlightPath fp(path);
    kmlsMutex.unlock();

    return fp;
}

std::vector<KMLFile> KMLWatcher::findKMLOnDrive() {
    char usr[256];
    getlogin_r(usr, 256);
    std::string r= SuperMisc::exec("find /media/"+std::string(usr)+" -name '*.kml'");

    if((r.find("find")==std::string::npos) and !r.empty()) {
        std::vector<std::string> lines = SuperMisc::split(r, '\n');
        std::vector<KMLFile> newSet;
        for(const std::string &l: lines){
            newSet.emplace_back(KMLFile(l));
        }
        return newSet;
    } else {
        return std::vector<KMLFile>();
    }
}

std::vector<KMLFile> KMLWatcher::findKMLLocal(const std::string &path) {
    std::string r= SuperMisc::exec("find "+path+" -name '*.kml'");

    if((r.find("find")==std::string::npos) and !r.empty()) {
        std::vector<std::string> lines = SuperMisc::split(r, '\n');
        std::vector<KMLFile> newSet;
        for(const std::string &l: lines)
            newSet.emplace_back(KMLFile(l));

        return newSet;
    } else {
        return std::vector<KMLFile>();
    }
}

int KMLWatcher::getKMLSetId() const {
    return kmlsSetId;
}

