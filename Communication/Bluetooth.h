//
// Created by dkulpa on 04.11.2022.
//

#ifndef PATHNAVER_BLUETOOTH_H
#define PATHNAVER_BLUETOOTH_H

// Threads lib
#include <thread>
#include <mutex>

// Bluetooth libs
#include <cstdio>
#include <unistd.h>
#include <sys/socket.h>
#include <bluetooth/bluetooth.h>
#include <bluetooth/l2cap.h>

typedef struct sockaddr_l2 sockaddr_rc_t;

class BluetoothRFCOMMServer {
public:
    BluetoothRFCOMMServer();

// Run mechanism
    bool start();
    void exec();
    void stop();

private:
    int soc;
    int cli;
    sockaddr_rc_t loc_addr;
    sockaddr_rc_t rem_addr;

    bool clientConnected;

    bool run;
};


#endif //PATHNAVER_BLUETOOTH_H
