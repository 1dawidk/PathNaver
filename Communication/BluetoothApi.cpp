//
// Created by dkulpa on 26.11.2022.
//


#include "BluetoothApi.h"


BluetoothApi::BluetoothApi(int port, uint32_t *uuid,
                           const std::string &name,
                           const std::string &descr,
                           const std::string &prov) {
    this->port= port;
    this->uuid= uuid;
    this->name= name;
    this->descr= descr;
    this->prov= prov;

    this->pairable = false;

    this->soc= -1;
    this->cli= -1;
    this->session= nullptr;
}

void BluetoothApi::start() {
    int result;
    struct sockaddr_rc loc_addr = { 0 };

    // local bluetooth adapter
    loc_addr.rc_family = AF_BLUETOOTH;
    loc_addr.rc_bdaddr = bdaddr_any;
    loc_addr.rc_channel = (uint8_t) port;

    // register service
    session= BluetoothApi::registerService(port, uuid, name, descr, prov);

    // allocate socket
    soc = socket(AF_BLUETOOTH, SOCK_STREAM, BTPROTO_RFCOMM);
    printf("socket() returned %d\n", soc);

    // bind socket to port 3 of the first available
    result = bind(soc, (struct sockaddr *)&loc_addr, sizeof(loc_addr));
    printf("bind() on channel %d returned %d\n", port, result);

    // put socket into listening mode
    result = listen(soc, 1);
    printf("listen() returned %d\n", result);

    apfd.fd = soc;
    apfd.events= POLLIN;
}

bool BluetoothApi::tryAccept(int timeout) {
    int cnt = poll(&apfd, 1, timeout);
    if(cnt>0) {
        if (apfd.revents & POLLIN) {
            socklen_t adrlen = sizeof(rem_addr);
            cli = accept(soc, (struct sockaddr *)&rem_addr, &adrlen);

            pfd.fd = cli;
            pfd.events = POLLIN | POLLRDHUP;

            return true;
        }
    }

    return false;
}

std::string BluetoothApi::getClientAddress() {
    char buffer[512] = { 0 };
    if(cli >= 0)
        ba2str(&rem_addr.rc_bdaddr, buffer);

    return buffer;
}

size_t BluetoothApi::tryRead(char *buf, int bufSize, int timeout) {
    size_t bytes_read= 0;

    if(cli >= 0) {
        int cnt = poll(&pfd, 1, timeout);
        if (cnt > 0) {
            if (pfd.revents & (POLLRDHUP | POLLHUP)) {
                cli = -1;
                bytes_read = 0;
            } else if (pfd.revents & POLLIN) {
                bytes_read = read(cli, buf, bufSize);
            }
        }
    } else {
        bytes_read = -1;
    }

    return bytes_read;
}

void BluetoothApi::write(const std::string &msg) const {
    if(cli >= 0)
        ::write(cli, msg.c_str(), msg.length());
}

bool BluetoothApi::isClientConnected() const {
    return cli >= 0;
}

void BluetoothApi::stop() {
    if(cli >= 0)
        close(cli);
    if(soc >= -1)
        close(soc);
    if(session != nullptr){
        sdp_close(session);
    }
}

BluetoothApi::~BluetoothApi() {
    finish();

    if(h!= nullptr){
        if(h->joinable())
            h->join();
        delete h;
    }
}


/** **************** STATIC METHODS **************** */

sdp_session_t *BluetoothApi::registerService(uint8_t rfcomm_channel,
                                             uint32_t *svc_uuid_int,
                                             const std::string &name,
                                             const std::string &descr,
                                             const std::string &prov) {

    uuid_t root_uuid, l2cap_uuid, rfcomm_uuid, svc_uuid{},
            svc_class_uuid;
    sdp_list_t *l2cap_list,
            *rfcomm_list,
            *root_list,
            *proto_list,
            *access_proto_list,
            *svc_class_list,
            *profile_list;
    sdp_data_t *channel;
    sdp_profile_desc_t profile;
    sdp_record_t record = { 0 };
    sdp_session_t *session;

    // set the general service ID
    sdp_uuid128_create(&svc_uuid, &svc_uuid_int);
    sdp_set_service_id(&record, svc_uuid);

    char str[256] = "";
    sdp_uuid2strn(&svc_uuid, str, 256);
    printf("Registering UUID %s\n", str);

    // set the service class
    sdp_uuid16_create(&svc_class_uuid, SERIAL_PORT_SVCLASS_ID);
    svc_class_list = sdp_list_append(nullptr, &svc_class_uuid);
    sdp_set_service_classes(&record, svc_class_list);

    // set the Bluetooth profile information
    sdp_uuid16_create(&profile.uuid, SERIAL_PORT_PROFILE_ID);
    profile.version = 0x0100;
    profile_list = sdp_list_append(nullptr, &profile);
    sdp_set_profile_descs(&record, profile_list);

    // make the service record publicly browsable
    sdp_uuid16_create(&root_uuid, PUBLIC_BROWSE_GROUP);
    root_list = sdp_list_append(nullptr, &root_uuid);
    sdp_set_browse_groups(&record, root_list);

    // set l2cap information
    sdp_uuid16_create(&l2cap_uuid, L2CAP_UUID);
    l2cap_list = sdp_list_append(nullptr, &l2cap_uuid);
    proto_list = sdp_list_append(nullptr, l2cap_list);

    // register the RFCOMM channel for RFCOMM sockets
    sdp_uuid16_create(&rfcomm_uuid, RFCOMM_UUID);
    channel = sdp_data_alloc(SDP_UINT8, &rfcomm_channel);
    rfcomm_list = sdp_list_append(nullptr, &rfcomm_uuid);
    sdp_list_append(rfcomm_list, channel);
    sdp_list_append(proto_list, rfcomm_list);

    access_proto_list = sdp_list_append(nullptr, proto_list);
    sdp_set_access_protos(&record, access_proto_list);

    // set the name, provider, and description
    sdp_set_info_attr(&record, name.c_str(), prov.c_str(), descr.c_str());

    // connect to the local SDP server, register the service record,
    // and disconnect
    session = sdp_connect(&bdaddr_any, &bdaddr_local, SDP_RETRY_IF_BUSY);
    sdp_record_register(session, &record, 0);

    // cleanup
    sdp_data_free(channel);
    sdp_list_free(l2cap_list, nullptr);
    sdp_list_free(rfcomm_list, nullptr);
    sdp_list_free(root_list, nullptr);
    sdp_list_free(access_proto_list, nullptr);
    sdp_list_free(svc_class_list, nullptr);
    sdp_list_free(profile_list, nullptr);

    return session;
}

void BluetoothApi::runPairable(int dur) {
    if(!pairable) {
        pairable = true;
        std::string result;
        std::string cmd = "python3 ../ParingListener.py -d=" + std::to_string(dur);
        paringPipe = new std::unique_ptr<FILE, decltype(&pclose)>(popen(cmd.c_str(), "r"), pclose);

        h = new std::thread(&BluetoothApi::_pairableWork, this);
    }
}

void BluetoothApi::_pairableWork() {
    std::array<char, 1024> buffer{};

    if(paringPipe!= nullptr) {
        while (fgets(buffer.data(), buffer.size(), paringPipe->get()) != nullptr) {
            // Wait
        }
    }
    pairable= false;
}

bool BluetoothApi::isPairableRunning() const {
    return pairable;
}

void BluetoothApi::finish() {
    if(cli>=0)
        close(cli);
    if(soc>=0)
        close(soc);

    sdp_close(session);
    session= nullptr;

    if(h != nullptr && h->joinable())
         h->join();
}
