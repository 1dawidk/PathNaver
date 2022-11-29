//
// Created by dkulpa on 26.11.2022.
//

#include <tim.h>
#include "WirelessSerialComm.h"
#include "../Console.h"

WirelessSerialComm::WirelessSerialComm() {
    rxMutex.unlock();
    txMutex.unlock();
}

[[maybe_unused]] std::string WirelessSerialComm::popMessage() {
    rxMutex.lock();
    std::string msg= blr.popMsg();
    rxMutex.unlock();

    return msg;
}

void WirelessSerialComm::onStart() {
    uint32_t uuid[4]= {0x01110000, 0x00100000, 0x80000080, 0xFB349B5F};
    btAPI= new BluetoothApi(1, uuid, "PathNaver", "Path naving device", "ILOT");
    btAPI->start();
}

void WirelessSerialComm::loop() {
    if(btAPI != nullptr){
        if(btAPI->isPairableRunning()){
            Console::logi("WSC", "Listening for pairing devices...");
            Worker::sleep(1000);
        } else {
            if(btAPI->isClientConnected()){
                char rxBuf[1024];
                size_t bytes_read = btAPI->tryRead(rxBuf, 1023, 100);
                if( bytes_read > 0 ) {
                    rxMutex.lock();
                    int c= blr.update(rxBuf);
                    rxMutex.unlock();
                    Console::logi("WSC", "Received "+std::to_string(c)+" new messages");
                } else {
                    txMutex.lock();
                    if (!sendQueue.empty()) {
                        std::string sendMsg = sendQueue[0];
                        sendQueue.erase(sendQueue.begin());
                        btAPI->write(sendMsg);
                    }
                    txMutex.unlock();

                    if (tim::now() - lastKASend > 4000) {
                        std::string kamsg = "$PNKPA*44\n";
                        btAPI->write(kamsg);
                        Console::logd("WSC", "Sending PNKPA");
                        lastKASend = tim::now();
                    }
                }
            } else {
                Console::logi("WSC", "Waiting for client...!");
                if (btAPI->tryAccept(5000)) {
                    Console::logi("WSC", "Client connected");
                }
            }
        }
    }
}

void WirelessSerialComm::makePairable() {
    if(btAPI!= nullptr)
        btAPI->runPairable(60);
}

bool WirelessSerialComm::isPairable() {
    if(btAPI!= nullptr)
        return btAPI->isPairableRunning();

    return true;
}

[[maybe_unused]] bool WirelessSerialComm::msgsInQueue() {
    rxMutex.lock();
    bool r= (blr.queueSize()>0);
    rxMutex.unlock();

    return r;
}

[[maybe_unused]] void WirelessSerialComm::send(const std::string &msg) {
    txMutex.lock();
    sendQueue.emplace_back(msg);
    txMutex.unlock();
}

[[maybe_unused]] bool WirelessSerialComm::isConnected() const {
    if(btAPI!= nullptr)
        return btAPI->isClientConnected();

    return false;
}

void WirelessSerialComm::onStop() {
    if(btAPI!= nullptr){
        btAPI->finish();
    }
    delete btAPI;
}
