//
// Created by dkulpa on 16.11.2022.
//

#ifndef PATHNAVER_CONSOLE_H
#define PATHNAVER_CONSOLE_H

#include <mutex>
#include "vector"
#include "string"
#include <thread>

class Console {
public:
    static void start();
    static void stop();
    static void exec();

    static void logi(const std::string &tag, const std::string &msg);
    static void loge(const std::string &tag, const std::string &msg);
    static void logw(const std::string &tag, const std::string &msg);
    static void logd(const std::string &tag, const std::string &msg);

private:
    static void log(const std::string &type, const std::string &tag, const std::string &msg);

    static std::vector<std::string> queue;
    static std::thread *runThd;
    static bool run;
    static std::mutex queueMutex;
};


#endif //PATHNAVER_CONSOLE_H
