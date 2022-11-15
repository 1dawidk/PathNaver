//
// Created by dkulpa on 07.11.2022.
//

#ifndef PATHNAVER_TCPSERIALCOMM_H
#define PATHNAVER_TCPSERIALCOMM_H

#include <netinet/in.h>
#include <sys/socket.h>
#include <iostream>
#include <arpa/inet.h>
#include <unistd.h>
#include "thread"
#include "cstring"

typedef struct sockaddr_in sockaddr_in_t;

class TCPSerialComm {
public:
    TCPSerialComm();
    bool start();
    void exec();


private:
    sockaddr_in_t loc;
    sockaddr_in_t rem;
    int soc;
    int cli;

    bool run;
    bool clientConnected;

    std::thread *kaThd;
};


#endif //PATHNAVER_TCPSERIALCOMM_H
