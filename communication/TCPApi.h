//
// Created by dkulpa on 07.11.2022.
//

#ifndef PATHNAVER_TCPAPI_H
#define PATHNAVER_TCPAPI_H

#include <netinet/in.h>
#include <sys/socket.h>
#include <iostream>
#include <arpa/inet.h>
#include <unistd.h>
#include <mutex>
#include "thread"
#include "cstring"
#include "BufferedReader.h"
#include "Worker.h"
#include <tim.h>
#include <sys/poll.h>
#include "../Console.h"

typedef struct sockaddr_in sockaddr_in_t;
typedef struct pollfd pollfd_t;

class TCPApi : public Worker{
public:
    explicit TCPApi();

    // Receive
    std::string popMessage();
    bool msgsInQueue();

    // Send
    void send(const std::string &msg);
    bool isConnected() const;

protected:
    void onStart() override;
    void loop() override;

private:
    sockaddr_in_t loc{};
    sockaddr_in_t rem{};
    int soc{};
    int cli{};
    pollfd_t pfd{};    // Messages listening pfd
    pollfd_t apfd{};   // Accept listening pfd

    ulong lastKASend{};
    ulong lastKARec{};

    bool saidWaiting{};

    BufferedReader br;
    std::vector<std::string> sendQueue;

    bool clientConnected{};

    std::mutex rxMutex;
    std::mutex txMutex;
};


#endif //PATHNAVER_TCPAPI_H
