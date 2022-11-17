//
// Created by dkulpa on 16.11.2022.
//

#ifndef PATHNAVER_DEVICECONFIG_H
#define PATHNAVER_DEVICECONFIG_H


#include <string>
#include <mutex>

#define RETURN_THREAD_SAFE(x, y, z) {(x).lock(); y v= z; (x).unlock(); return v;}
#define SET_THREAD_SAFE(x, y, z) {(x).lock(); (y)=z; (x).unlock();}


/**
 *  Thread safe configuration container
 */

class DeviceConfig {
public:
    DeviceConfig();

    void setSelectedKml(const std::string &v);
    std::string getSelectedKml();
private:
    std::string selectedKml;
    std::mutex changeMutex;
};


#endif //PATHNAVER_DEVICECONFIG_H
