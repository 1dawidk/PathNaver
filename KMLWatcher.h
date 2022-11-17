//
// Created by dkulpa on 16.11.2022.
//

#ifndef PATHNAVER_KMLWATCHER_H
#define PATHNAVER_KMLWATCHER_H


#include <vector>
#include <mutex>
#include "Worker.h"
#include "string"

class KMLWatcher : public Worker{
public:
    int countFound();
    std::string getFileName(int idx);

protected:
    void onStart() override;
    void loop() override;

private:
    std::vector<std::string> kmls;
    std::mutex kmlsMutex;
};


#endif //PATHNAVER_KMLWATCHER_H
