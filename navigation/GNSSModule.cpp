//
// Created by dkulpa on 29.07.2022.
//

#include <NMEA/NMEA.h>
#include "GNSSModule.h"
#include "../Geo/GeoCalc.h"
#include "../Console.h"

GNSSModule::GNSSModule(int baudrate, bool verbose, const std::string &port) {
    this->baudrate = baudrate;
    this->verbose = verbose;
    this->port = port;
    this->buffer_started = false;
    this->serial_buffer = nullptr;
    state = GNSS_MODULE_NOT_FOUND;

    dataMutex.unlock();

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
                                         false);

    return !this->port.empty();
}

bool GNSSModule::startBuffer() {
    if(this->port.empty()){
        this->find();
    }

    if(!this->port.empty()){
        this->serial_buffer = new SerialBuffer(this->port, this->baudrate, 2048);
        this->buffer_started = true;
        return true;
    }

    return false;
}

bool GNSSModule::hasFix() {
    dataMutex.lock();
    bool f= (state==GNSS_MODULE_OK);
    dataMutex.unlock();

    return f;
}

GNSSData GNSSModule::getData() {
    dataMutex.lock();
    GNSSData d = gnssData;
    dataMutex.unlock();

    return d;
}

void GNSSModule::cleanup() {
    dataMutex.lock();
    state= GNSS_MODULE_NOT_FOUND;
    dataMutex.unlock();

    if(serial_buffer != nullptr) {
        if (this->verbose)
            Console::logi("GNSSModule", "Closing serial...");
        serial_buffer->close();
        delete serial_buffer;
        serial_buffer = nullptr;
    }
    buffer_started= false;

    if(!fixed_port)
        this->port = "";
}

void GNSSModule::loop() {
    // Serial open
    if((this->serial_buffer!= nullptr) && serial_buffer->isOpen() && buffer_started) {
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

                    double llat= gnssData.getLat();
                    double llng= gnssData.getLng();
                    dataMutex.lock();
                    gnssData.setLat( (double)latd + latm/60.0 );
                    gnssData.setLng((double)lngd + lngm/60.0);
                    gnssData.setHeading(GeoCalc::calcHeading(GeoPoint(llat, llng), GeoPoint(gnssData.getLat(), gnssData.getLng())));
                    state= GNSS_MODULE_OK;
                    dataMutex.unlock();
                } else {
                    dataMutex.lock();
                    state= GNSS_MODULE_NO_FIX;
                    dataMutex.unlock();
                }
            } catch (std::invalid_argument &e) {
                if (this->verbose)
                    Console::loge("GNSSModule", e.what());
            } catch (std::out_of_range &e) {
                if (this->verbose)
                    Console::loge("GNSSModule", e.what());
            }
        } else {
            // Check if port is really open. If not -> perform cleanup
            if(!serial_buffer->isOpen()){
                dataMutex.lock();
                state= GNSS_MODULE_NOT_FOUND;
                dataMutex.unlock();
                Console::logw("GNSSModule", "Disconnected! Cleanup!");
                cleanup();
                Worker::sleep(3000);
            } else {
                dataMutex.lock();
                state= GNSS_MODULE_NO_FIX;
                dataMutex.unlock();
            }
        }
    }
    // Serial not open
    else {
        cleanup();
        if(!startBuffer()){
            dataMutex.lock();
            state= GNSS_MODULE_NOT_FOUND;
            dataMutex.unlock();
            Console::loge("GNSSModule", "Cannot open gnss module serial!");
            Worker::sleep(3000);
        }
    }

    Worker::sleep(50);
}

void GNSSModule::onStop() {
    cleanup();
}

int GNSSModule::getState() const {
    return state;
}
