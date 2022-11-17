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
#include <mutex>
#include "thread"
#include "cstring"
#include "BufferedReader.h"

typedef struct sockaddr_in sockaddr_in_t;

class TCPSerialComm {
public:
    TCPSerialComm();
    bool start();
    void exec();

    // Receive
    std::string popMessage();
    bool msgsInQueue();

    // Send
    void send(const std::string &msg);

private:
    sockaddr_in_t loc;
    sockaddr_in_t rem;
    int soc;
    int cli;

    BufferedReader br;
    std::vector<std::string> sendQueue;

    bool run;
    bool clientConnected;

    std::mutex rxMutex;
    std::mutex txMutex;
    std::thread *kaThd;
};


#endif //PATHNAVER_TCPSERIALCOMM_H
