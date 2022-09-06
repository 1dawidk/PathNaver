//
// Created by dkulpa on 18.05.2022.
//

#include "SerialBuffer.h"



SerialBuffer::SerialBuffer(const std::string& port, unsigned int baudrate, int maxlen) {
    this->serial = new Serial(port, baudrate);
    this->serial->start();
    this->maxlen = maxlen;
}

/**
 * Read serial and serial_buffer received data. Trims serial_buffer if it is longer than max length.
 * @return If serial_buffer contains line, returns first line found in internal serial_buffer
 * (without newline character).
 */
std::string SerialBuffer::update() {
    std::string ret;

    std::string rec = this->serial->receive();
    this->buffer.append(rec);
    // If serial_buffer is too long -> erase beginning
    if(this->buffer.length() > this->maxlen)
        this->buffer.erase(0, this->buffer.length()-maxlen);

    // Find new line character
    size_t nl = this->buffer.find('\n');
    if(nl != std::string::npos){
        // Get first found line
        ret = this->buffer.substr(0, nl);
        this->buffer.erase(0, nl+1);
    }

    return ret;
}

bool SerialBuffer::isOpen() {
    return this->serial->isOpen();
}

void SerialBuffer::close() {
    this->serial->stop();
    buffer= "";
}
