//
// Created by dkulpa on 07.11.2022.
//
#include "TCPApi.h"


TCPApi::TCPApi() {
    rxMutex.unlock();
    txMutex.unlock();
}

void TCPApi::onStart() {
    int opt = 1;

    // Creating socket file descriptor
    if ((soc = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        Console::loge("TSC", "Cannot create socket!");
    }

    // Forcefully attaching socket to the port 8080
    if (setsockopt(soc, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt))) {
        Console::loge("TSC", "Cannot set opts!");
    }
    loc.sin_family = AF_INET;
    loc.sin_addr.s_addr = INADDR_ANY;
    loc.sin_port = htons(12345);

    // Forcefully attaching socket
    if (bind(soc, (struct sockaddr*)&loc, sizeof(loc)) < 0) {
        Console::loge("TSC", "Cannot bind!");
    }
    if (listen(soc, 3) < 0) {
        Console::loge("TSC", "Cannot create listen!");
    }

    apfd.fd = soc;
    apfd.events= POLLIN;

    clientConnected= false;
    saidWaiting= false;
}

void TCPApi::loop() {
    if(clientConnected){
        int cnt = poll(&pfd, 1, 100);
        if(cnt>0){

            if(pfd.revents & (POLLRDHUP | POLLHUP)){
                clientConnected = false;
                Console::logi("TSC", "Client disconnected!");
            } else if(pfd.revents & POLLIN){
                char rxBuf[1024];
                size_t bytes_read = read(cli, rxBuf, sizeof(rxBuf)-1);
                if( bytes_read > 0 ) {
                    rxMutex.lock();
                    int c= br.update(rxBuf);
                    rxMutex.unlock();
                    Console::logi("TSC", "Received "+std::to_string(c)+" new messages");
                }
            }
        } else {
            txMutex.lock();
            if (!sendQueue.empty()) {
                std::string sendMsg = sendQueue[0];
                sendQueue.erase(sendQueue.begin());

                write(cli, sendMsg.c_str(), sendMsg.length());
            }
            txMutex.unlock();

            if (tim::now() - lastKASend > 4000) {
                std::string kamsg = "$PNKPA*44\n";
                write(cli, kamsg.c_str(), kamsg.length());
                Console::logd("TSC", "Sending PNKPA");
                lastKASend = tim::now();
            }
        }

        //if(tim::now()-lastKARec>10000){
        //  clientConnected= false;
        //close(cli);
        //}
    } else {
        //if(!saidWaiting) {
            Console::logi("TSC", "Waiting for client...!");
            saidWaiting = true;
        //}
        int cnt = poll(&apfd, 1, 5000);
        if(cnt>0) {
            if (apfd.revents & POLLIN) {
                int adrlen = sizeof(loc);
                cli = accept(soc, (struct sockaddr *) &rem, (socklen_t *) &adrlen);
                char buf[128];
                inet_ntop(AF_INET, &rem.sin_addr, buf, sizeof(rem));
                Console::logi("TSC", "New client connected (" + std::string(buf) + ")");
                lastKASend = tim::now();
                lastKARec = tim::now();

                pfd.fd = cli;
                pfd.events = POLLIN | POLLRDHUP;

                clientConnected = true;
                saidWaiting = false;
            }
        }
    }
}


std::string TCPApi::popMessage() {
    rxMutex.lock();
    std::string msg= br.popMsg();
    rxMutex.unlock();

    return msg;
}

bool TCPApi::msgsInQueue() {
    rxMutex.lock();
    bool r= (br.queueSize()>0);
    rxMutex.unlock();

    return r;
}

void TCPApi::send(const std::string &msg) {
    txMutex.lock();
    sendQueue.emplace_back(msg);
    txMutex.unlock();
}

bool TCPApi::isConnected() const {
    return clientConnected;
}

