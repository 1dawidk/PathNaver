//
// Created by dkulpa on 18.05.2022.
//

#include <sys/stat.h>
#include "Serial.h"

Serial::Serial(const std::string &port, unsigned int baudrate) {
    this->fd = -1;
    this->port= port;
    this->baudrate= Serial::translate_baudrate(baudrate);
}

int Serial::start() {
    if(this->baudrate < 0){
        std::cout << "Wrong baudrate value" << std::endl;
        return -3;
    }

    this->fd = open(port.c_str(), O_RDWR | O_NOCTTY | O_NONBLOCK);		//Open in non blocking read/write mode
    if (this->fd == -1) {
        std::cout << "Cannot open port " << port << std::endl;
        return -1;
    }

    if( configPort()!=0 ){
        std::cout << "Cannot configure port " << port << std::endl;
        return -2;
    }

    return 0;
}

int Serial::configPort() const {
    struct termios options{};

    if(tcgetattr(fd, &options)!=0) {
        return -1;
    }

    // Set baud rate
    cfsetispeed(&options, baudrate);
    cfsetospeed(&options, baudrate);

    /* Setting other Port Stuff */
    options.c_cflag &= ~(PARENB | CSTOPB | CSIZE | CRTSCTS);            // Make 8n1
    options.c_cflag |= CS8 | CREAD | CLOCAL;
    options.c_oflag &= ~OPOST;
    options.c_oflag &= ~ONLCR;

    /* Flush Port, then apply attributes */
    tcflush(fd, TCIFLUSH);
    if (tcsetattr(fd, TCSANOW, &options)!=0) {
        return -2;
    }


    return 0;
}

void Serial::transmit(const std::string &msg) const {
    if (this->fd != -1){
        write(this->fd, msg.c_str(), msg.length());
    }
}

std::string Serial::receive() const {
    if (this->fd != -1) {
        char r[512];
        memset(r, '\0', sizeof r);

        size_t rlen = read(this->fd, r, 500);		//Filestream, serial_buffer to store in, number of bytes to read (max)
        if (rlen <= 0){
            return "";
        } else {
            return std::string(r);
        }

    }

    return "";
}

void Serial::stop() {
    if(this->fd != -1){
        close(this->fd);
        this->fd= -1;
    }
}

Serial::~Serial() {
    this->stop();
}

bool Serial::isOpen() const {
    struct termios opts{};
    return ((fcntl(fd, F_GETFL) != -1) and (tcgetattr(fd, &opts) != -1));
}

unsigned int Serial::translate_baudrate(unsigned int raw) {
    switch (raw) {
        case 0: return B0;
        case 50: return B50;
        case 75: return B75;
        case 110: return B110;
        case 134: return B134;
        case 150: return B150;
        case 200: return B200;
        case 300: return B300;
        case 600: return B600;
        case 1200: return B1200;
        case 1800: return B1800;
        case 2400: return B2400;
        case 4800: return B4800;
        case 9600: return B9600;
        case 19200: return B19200;
        case 38400: return B38400;
        case 57600: return B57600;
        case 115200: return B115200;
        case 230400: return B230400;
        case 460800: return B460800;
        case 500000: return B500000;
        case 576000: return B576000;
        case 921600: return B921600;
        case 1000000: return B1000000;
        case 1152000: return B1152000;
        case 1500000: return B1500000;
        case 2000000: return B2000000;
        case 3000000: return B3000000;
        case 3500000: return B3500000;
        case 4000000: return B4000000;
        default: return -1;
    }
}
