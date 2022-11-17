//
// Created by dkulpa on 16.11.2022.
//

#include <iostream>
#include <thread>
#include "Console.h"
#include "tim.h"

std::vector<std::string> Console::queue;
std::thread *Console::runThd;
bool Console::run;
std::mutex Console::queueMutex;

void Console::logi(const std::string &tag, const std::string &msg) {
    Console::log("[INFO]", tag, msg);
}

void Console::loge(const std::string &tag, const std::string &msg) {
    Console::log("[ERR]", tag, msg);
}

void Console::logw(const std::string &tag, const std::string &msg) {
    Console::log("[WRN]", tag, msg);
}

void Console::logd(const std::string &tag, const std::string &msg) {
    Console::log("[DBG]", tag, msg);
}

void Console::log(const std::string &type, const std::string &tag, const std::string &msg) {
    queueMutex.lock();
    queue.emplace_back(tim::format(tim::now(), false, true, true)
            + "\t" + type + "\t" + tag + "\t" + msg);
    queueMutex.unlock();
}

void Console::start() {
    run= true;
    queueMutex.unlock();
    runThd= new std::thread(Console::exec);
}

void Console::stop() {
    run= false;
    runThd->join();
    delete runThd;
}

void Console::exec() {
    while(run){
        queueMutex.lock();
        if(!queue.empty()) {
            std::cout << queue[0] << std::endl;
            queue.erase(queue.begin());
        }
        queueMutex.unlock();
        std::this_thread::sleep_for(std::chrono::microseconds(1000*10));
    }
}




