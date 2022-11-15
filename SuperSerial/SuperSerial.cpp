//
// Created by dkulpa on 18.05.2022.
//

#include "SuperSerial.h"

std::string SuperSerial::findDevice(int baudrate,
                                    const std::string &tag,
                                    const std::string &name,
                                    bool verbose) {
    std::string devices_port;

    if(verbose)
        std::cout << "[INFO] Looking for " << name << "..." << std::endl;

    std::vector<std::string> ports = SuperSerial::listPorts();
    Serial *serial;
    for(auto & port : ports){
        if(verbose)
            std::cout << "\tListening " << port << "." << std::flush;

        // Open serial port
        serial = new Serial(port, baudrate);
        // Start port
        if(!serial->start()) {
            // Wait for response
            std::string resp;
            for (int j = 0; j < 5; j++) {
                resp.append(serial->receive());
                if (resp.find(tag) != std::string::npos) {
                    devices_port = port;
                    break;
                }
                tim::delay(200);
                if (verbose)
                    std::cout << "." << std::flush;
            }

            // Stop serial port and delete serial object (cleanup)
            serial->stop();
            delete serial;

            // If device's port found -> break loop
            if (!devices_port.empty()) {
                if (verbose)
                    std::cout << "  BINGO!" << std::endl;
                break;
            } else {
                if (verbose)
                    std::cout << "  not in here :(" << std::endl;
            }
        } else { // if port could not be opened
            // Cleanup and go to next port
            delete serial;
            if (verbose)
                std::cout << "[WARN] can't open " << port << "!" << std::endl;
        }
    }

    return devices_port;
}

std::string
SuperSerial::findDevice(int baudrate,
                        const std::vector<std::string> &tags,
                        const std::string &name,
                        bool verbose) {
    std::string devices_port;

    if(verbose)
        std::cout << "[INFO] Looking for " << name << "..." << std::endl;

    std::vector<std::string> ports = SuperSerial::listPorts();
    Serial *serial;
    for(auto & port : ports){
        if(verbose)
            std::cout << "\tListening " << port << "." << std::flush;

        // Open serial port
        serial = new Serial(port, baudrate);
        // Start port
        if(!serial->start()) {
            // Wait for response
            std::string resp;
            for (int j = 0; j < 5; j++) {
                resp.append(serial->receive());
                for(auto &tag : tags) {
                    if (resp.find(tag) != std::string::npos) {
                        devices_port = port;
                    }
                }
                if(!devices_port.empty())
                    break;
                tim::delay(200);
                if (verbose)
                    std::cout << "." << std::flush;
            }

            // Stop serial port and delete serial object (cleanup)
            serial->stop();
            delete serial;

            // If device's port found -> break loop
            if (!devices_port.empty()) {
                if (verbose)
                    std::cout << "  BINGO!" << std::endl;
                break;
            } else {
                if (verbose)
                    std::cout << "  not in here :(" << std::endl;
            }
        } else { // if port could not be opened
            // Cleanup and go to next port
            delete serial;
            if (verbose)
                std::cout << "[WARN] can't open " << port << "!" << std::endl;
        }
    }

    return devices_port;
}

std::string SuperSerial::findDeviceWithPoke(int baudrate,
                                            const std::string &poke,
                                            const std::string &answer,
                                            const std::string &name,
                                            bool verbose) {
    std::string devices_port;

    if(verbose)
        std::cout << "[INFO] Looking for " << name << " using poke..." << std::endl;

    std::vector<std::string> ports = SuperSerial::listPorts();
    Serial *serial;
    for(auto & port : ports){
        if(verbose)
            std::cout << "\tPoking " << port << "." << std::flush;

        // Open serial port
        serial = new Serial(port, baudrate);
        // Start port
        if(!serial->start()) {
            // Transmit poke text
            serial->transmit(poke);
            tim::delay(10);

            // Wait for response
            std::string resp;
            for (int j = 0; j < 3; j++) {
                resp.append(serial->receive());
                if (resp.find(answer) != std::string::npos) {
                    devices_port = port;
                    break;
                }
                tim::delay(300);
                if (verbose)
                    std::cout << "." << std::flush;
            }

            // Stop serial port and delete serial object (cleanup)
            serial->stop();
            delete serial;

            // If device's port found -> break loop
            if (!devices_port.empty()) {
                if (verbose)
                    std::cout << "  BINGO!" << std::endl;
                break;
            } else {
                if (verbose)
                    std::cout << "  not in here :(" << std::endl;
            }
        } else { // if port could not be opened
            // Cleanup and go to next port
            delete serial;
            if (verbose)
                std::cout << "[WARN] can't open " << port << "!" << std::endl;
        }
    }

    return devices_port;
}

std::vector<std::string> SuperSerial::listPorts() {
    std::vector<std::string> ports;

    std::string r = SuperMisc::exec("ls -1 /dev/ttyACM* 2> /dev/null");
    if(!r.empty()){
        std::vector<std::string> ACMs = SuperMisc::split(r,'\n');
        ports.insert(ports.end(), ACMs.begin(), ACMs.end());
    }

    r = SuperMisc::exec("ls -1 /dev/ttyUSB* 2> /dev/null");
    if(!r.empty()) {
        std::vector<std::string> USBs = SuperMisc::split(r, '\n');
        ports.insert(ports.end(), USBs.begin(), USBs.end());
    }

    r = SuperMisc::exec("ls -1 /dev/ttyS* 2> /dev/null");
    if(!r.empty()) {
        std::vector<std::string> Ss = SuperMisc::split(r, '\n');
        ports.insert(ports.end(), Ss.begin(), Ss.end());
    }

    return ports;
}