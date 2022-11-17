//
// Created by dkulpa on 16.11.2022.
//

#include "Worker.h"

Worker::Worker() {
    run = false;
    h = nullptr;
}

void Worker::start() {
    run = true;
    h = new std::thread(&Worker::_exec, this);
}

void Worker::_exec() {
    onStart();
    while(run){
        loop();
    }
}

Worker::~Worker() {
    stop();
    delete h;
}

void Worker::stop() {
    run = false;
    if(h != nullptr)
        h->join();
}

void Worker::onStart() {

}
