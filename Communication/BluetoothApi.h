//
// Created by dkulpa on 26.11.2022.
//

#ifndef PATHNAVER_BLUETOOTHAPI_H
#define PATHNAVER_BLUETOOTHAPI_H

#include <bluetooth/bluetooth.h>
#include <bluetooth/sdp.h>
#include <bluetooth/sdp_lib.h>
#include <bluetooth/rfcomm.h>
#include <string>
#include <sys/poll.h>
#include <unistd.h>
#include <memory>
#include <stdexcept>
#include <string>
#include <array>
#include <thread>

typedef struct pollfd pollfd_t;

class BluetoothApi {
public:
    BluetoothApi(int port,
                 uint32_t *svc_uuid_int,
                 const std::string &name,
                 const std::string &descr,
                 const std::string &prov);
    void start();
    bool tryAccept(int timeout);
    size_t tryRead(char *buf, int bufSize, int timeout);
    void write(const std::string &msg) const;
    [[nodiscard]] bool isClientConnected() const;
    void finish();


    void runPairable(int dur);
    bool isPairableRunning() const;
    void _pairableWork();

    std::string getClientAddress();

    void stop();
    ~BluetoothApi();

private:
    std::string name;
    std::string descr;
    std::string prov;
    uint32_t *uuid;

    int soc;
    sdp_session_t *session;
    int port;

    bool pairable;
    std::unique_ptr<FILE, decltype(&pclose)> *paringPipe{};
    std::thread *h{};

    int cli;
    struct sockaddr_rc rem_addr = { 0 };

    pollfd_t pfd{};    // Messages listening pfd
    pollfd_t apfd{};   // Accept listening pfd

    static sdp_session_t* registerService(uint8_t rfcomm_channel,
                                          uint32_t *svc_uuid_int,
                                          const std::string &name,
                                          const std::string &descr,
                                          const std::string &prov);

    constexpr static bdaddr_t bdaddr_any{{0, 0, 0, 0, 0, 0}};
    constexpr static bdaddr_t bdaddr_local{{0, 0, 0, 0xff, 0xff, 0xff}};
};


#endif //PATHNAVER_BLUETOOTHAPI_H
