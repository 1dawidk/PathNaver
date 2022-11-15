//
// Created by dkulpa on 29.07.2022.
//

#include <NMEA/NMEA.h>
#include "GNSSModule.h"
#include "Geo/GeoCalc.h"

GNSSModule::GNSSModule(int baudrate, bool verbose, const std::string &port) {
    this->baudrate = baudrate;
    this->verbose = verbose;
    this->port = port;
    this->buffer_started = false;
    this->serial_buffer = nullptr;
    this->lat = 52.1799270067;
    this->lng = 20.94973112;
    this->heading = 0;
    this->fix = false;

    if(port.empty()){
        fixed_port = false;
    } else {
        fixed_port = true;
    }
}

bool GNSSModule::find() {
    std::vector<std::string> tags;
    tags.emplace_back("$GPRMC");
    tags.emplace_back("$GNRMC");

    this->port = SuperSerial::findDevice(this->baudrate,
                                         tags,
                                         "GNSS module",
                                         true);

    return !this->port.empty();
}

void GNSSModule::startBuffer() {
    if(this->port.empty()){
        this->find();
    }

    if(!this->port.empty()){
        this->serial_buffer = new SerialBuffer(this->port, this->baudrate, 2048);
        this->buffer_started = true;
    }
}

void GNSSModule::update() {
    if((this->serial_buffer!= nullptr) && buffer_started) {
        std::string ret = this->serial_buffer->update();
        if (!ret.empty()) {
            try {
                SuperMisc::removeChar(ret, '\n');
                SuperMisc::removeChar(ret, '\r');

                std::vector<std::string> data = SuperMisc::split(ret, ',');
                if(data.size()>=11 and data[0]=="$GPRMC" and data[2]=="A" and !data[3].empty() and !data[5].empty() and !data[8].empty()){
                    int latd = std::stoi(data[3].substr(0, 2));
                    double latm = std::stod(data[3].substr(2));
                    int lngd = std::stoi(data[5].substr(0, 3));
                    double lngm = std::stod(data[5].substr(3));

                    double llat= lat;
                    double llng= lng;
                    this->lat = (double)latd + latm/60.0;
                    this->lng = (double)lngd + lngm/60.0;
                    //this->heading = std::stod(data[8]);
                    this->heading = GeoCalc::calcHeading(GeoPoint(llat, llng), GeoPoint(lat, lng));
                    //std::cout << "[DEBUG] " << "GNSSModule: update - " << lat << "," << lng << std::endl;
                    this->fix = true;
                } else {
                    this->fix = false;
                }
            } catch (std::invalid_argument &e) {
                if (this->verbose)
                    std::cout << e.what() << std::endl;
            } catch (std::out_of_range &e) {
                if (this->verbose)
                    std::cout << e.what() << std::endl;
            }
        } else {
            // Check if port is really open. If not -> perform cleanup
            if(!serial_buffer->isOpen()){
                std::cout << "[WARN] GNSS module disconnected! Cleanup!" << std::endl;
                cleanup();
            }
        }
    }
}

bool GNSSModule::isOpen() const {
    return (serial_buffer!= nullptr) && buffer_started && serial_buffer->isOpen();
}

double GNSSModule::getLat() const {
    return lat;
}

double GNSSModule::getLng() const {
    return lng;
}

double GNSSModule::getHeading() const {
    return heading;
}

bool GNSSModule::hasFix() const {
    return fix;
}

void GNSSModule::cleanup() {
    this->fix= false;
    if(serial_buffer != nullptr) {
        std::cout << "[INFO] Closing serial..." << std::endl;
        serial_buffer->close();
        delete serial_buffer;
    }
    buffer_started= false;

    if(!fixed_port)
        this->port = "";
}
