//
// Created by dkulpa on 16.11.2022.
//

#ifndef PATHNAVER_WORKER_H
#define PATHNAVER_WORKER_H


#include <thread>

class Worker {
public:
    Worker();

    void start();
    void stop();

    ~Worker();

protected:
    virtual void onStart();
    virtual void loop() = 0;

private:
    std::thread *h;
    void _exec();
    bool run;
};


#endif //PATHNAVER_WORKER_H
