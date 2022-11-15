//
// Created by dkulpa on 07.11.2022.
//


#include <tim.h>
#include <sys/poll.h>
#include "TCPSerialComm.h"
#include "BufferedReader.h"

TCPSerialComm::TCPSerialComm() {
    run= false;
}

bool TCPSerialComm::start() {
    int opt = 1;

    // Creating socket file descriptor
    if ((soc = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        std::cout << "Cannot create socket!" << std::endl;
        return false;
    }

    // Forcefully attaching socket to the port 8080
    if (setsockopt(soc, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt))) {
        std::cout << "Cannot set opts!" << std::endl;
        return false;
    }
    loc.sin_family = AF_INET;
    loc.sin_addr.s_addr = INADDR_ANY;
    loc.sin_port = htons(12345);

    // Forcefully attaching socket
    if (bind(soc, (struct sockaddr*)&loc, sizeof(loc)) < 0) {
        std::cout << "Cannot bind!" << std::endl;
        return false;
    }
    if (listen(soc, 3) < 0) {
        std::cout << "Cannot listen!" << std::endl;
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
    BufferedReader br;

    while(run){
        if(clientConnected){
            int cnt = poll(&pfd, 1, 100);
            if(cnt>0){
                if(pfd.revents & POLLIN){
                    size_t bytes_read = read(cli, rxBuf, sizeof(rxBuf)-1);
                    if( bytes_read > 0 ) {
                        int c= br.update(rxBuf);
                        std::cout << "TSC: Received " << c << " new messages" << std::endl;
                    }
                }
            }

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
            std::cout << "TSC: Waiting for client..." << std::endl;
            int adrlen = sizeof(loc);
            cli = accept(soc, (struct sockaddr*)&rem, (socklen_t *)&adrlen);
            char buf[1024];
            inet_ntop(AF_INET, &rem.sin_addr, buf, sizeof(rem));
            std::cout << "TSC: New client connected: " << std::string(buf) << std::endl;
            lastKASend= tim::now();
            lastKARec= tim::now();

            pfd.fd = cli;
            pfd.events = POLLIN;

            clientConnected= true;
        }
    }
}

