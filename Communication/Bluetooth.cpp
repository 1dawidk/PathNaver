//
// Created by dkulpa on 04.11.2022.
//

#include <iostream>
#include "Bluetooth.h"

BluetoothRFCOMMServer::BluetoothRFCOMMServer() {
    run = false;
}

bool BluetoothRFCOMMServer::start() {
    loc_addr = { 0 };
    rem_addr = { 0 };
    socklen_t opt = sizeof(rem_addr);

    // allocate socket
    soc = socket(AF_BLUETOOTH, SOCK_SEQPACKET, BTPROTO_L2CAP);

    // bind socket to port 0x1001 of the first available
    // bluetooth adapter
    loc_addr.l2_family = AF_BLUETOOTH;
    loc_addr.l2_bdaddr = {0, 0, 0, 0 ,0 ,0};
    loc_addr.l2_psm = htobs(0x1001);

    bind(soc, (struct sockaddr *)&loc_addr, sizeof(loc_addr));

    // put socket into listening mode
    listen(soc, 1);
}

void BluetoothRFCOMMServer::exec() {
    while(run){
        if(clientConnected){
            char buf[1024];
            size_t bytes_read = read(cli, buf, sizeof(buf));
            if( bytes_read > 0 ) {
                printf("received [%s]\n", buf);
            }
        } else {
            socklen_t opt = sizeof(rem_addr);
            std::cout << "Bluetooth: Waiting for client..." << std::endl;
            cli = accept(soc, (struct sockaddr *)&rem_addr, &opt);
            char buf[1024];
            ba2str( &rem_addr.l2_bdaddr, buf );
            std::cout << "Bluetooth: New client connected: " << std::string(buf) << std::endl;
        }
    }
}

void BluetoothRFCOMMServer::stop() {
    close(cli);
    close(soc);
    this->run = false;
}


