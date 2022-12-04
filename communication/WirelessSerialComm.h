//
// Created by dkulpa on 26.11.2022.
//

#ifndef PATHNAVER_WIRELESSSERIALCOMM_H
#define PATHNAVER_WIRELESSSERIALCOMM_H

#include <Worker.h>
#include <mutex>
#include "BluetoothApi.h"
#include "BufferedReader.h"

class WirelessSerialComm : public Worker {
public:
    explicit WirelessSerialComm();

    // Receive
    [[maybe_unused]] std::string popMessage();
    [[maybe_unused]] bool msgsInQueue();

    // Send
    [[maybe_unused]] void send(const std::string &msg);
    [[maybe_unused]] [[nodiscard]] bool isConnected() const;

    void makePairable();
    bool isPairable();


protected:
    void onStart() override;
    void loop() override;
    void onStop() override;

private:
    BluetoothApi *btAPI= nullptr;

    ulong lastKASend{};
    ulong lastKARec{};

    BufferedReader blr;
    std::vector<std::string> sendQueue;

    std::mutex rxMutex;
    std::mutex txMutex;
};


#endif //PATHNAVER_WIRELESSSERIALCOMM_H
