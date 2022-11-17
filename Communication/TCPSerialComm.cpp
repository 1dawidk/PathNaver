//
// Created by dkulpa on 07.11.2022.
//


#include <tim.h>
#include <sys/poll.h>
#include "TCPSerialComm.h"
#include "../Console.h"

TCPSerialComm::TCPSerialComm() {
    run= false;
    rxMutex.unlock();
    txMutex.unlock();
}

bool TCPSerialComm::start() {
    int opt = 1;

    // Creating socket file descriptor
    if ((soc = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        Console::loge("TSC", "Cannot create socket!");
        return false;
    }

    // Forcefully attaching socket to the port 8080
    if (setsockopt(soc, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt))) {
        Console::loge("TSC", "Cannot set opts!");
        return false;
    }
    loc.sin_family = AF_INET;
    loc.sin_addr.s_addr = INADDR_ANY;
    loc.sin_port = htons(12345);

    // Forcefully attaching socket
    if (bind(soc, (struct sockaddr*)&loc, sizeof(loc)) < 0) {
        Console::loge("TSC", "Cannot bind!");
        return false;
    }
    if (listen(soc, 3) < 0) {
        Console::loge("TSC", "Cannot create listen!");
        return false;
    }

    run= true;
    clientConnected= false;
    return true;
}

void TCPSerialComm::exec() {
    ulong lastKASend;
    ulong lastKARec;
    struct pollfd pfd{};

    char rxBuf[1024];

    while(run){
        if(clientConnected){
            int cnt = poll(&pfd, 1, 100);
            if(cnt>0){
                if(pfd.revents & POLLIN){
                    size_t bytes_read = read(cli, rxBuf, sizeof(rxBuf)-1);
                    if( bytes_read > 0 ) {
                        rxMutex.lock();
                        int c= br.update(rxBuf);
                        rxMutex.unlock();
                        Console::loge("TSC", "Received "+std::to_string(c)+" new messages");
                    }
                }
            }

            txMutex.lock();
            if(!sendQueue.empty()){
                std::string sendMsg= sendQueue[0];
                sendQueue.erase(sendQueue.begin());

                write(cli, sendMsg.c_str(), sendMsg.length());
            }
            txMutex.unlock();

            if(tim::now()-lastKASend>4000){
                std::string kamsg= "$PNKPA*44\n";
                write(cli, kamsg.c_str(), kamsg.length());
                lastKASend= tim::now();
            }

            //if(tim::now()-lastKARec>10000){
              //  clientConnected= false;
                //close(cli);
            //}
        } else {
            Console::loge("TSC", "Waiting for client...!");
            int adrlen = sizeof(loc);
            cli = accept(soc, (struct sockaddr*)&rem, (socklen_t *)&adrlen);
            char buf[128];
            inet_ntop(AF_INET, &rem.sin_addr, buf, sizeof(rem));
            Console::loge("TSC", "New client connected ("+std::string(buf)+")");
            lastKASend= tim::now();
            lastKARec= tim::now();

            pfd.fd = cli;
            pfd.events = POLLIN;

            clientConnected= true;
        }
    }
}

std::string TCPSerialComm::popMessage() {
    rxMutex.lock();
    std::string msg= br.popMsg();
    rxMutex.unlock();

    return msg;
}

bool TCPSerialComm::msgsInQueue() {
    rxMutex.lock();
    bool r= (br.queueSize()>0);
    rxMutex.unlock();

    return r;
}

void TCPSerialComm::send(const std::string &msg) {
    txMutex.lock();
    sendQueue.emplace_back(msg);
    txMutex.unlock();
}

