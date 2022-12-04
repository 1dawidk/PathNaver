//
// Created by dkulpa on 18.05.2022.
//

#include "NMEA.h"

NMEA::NMEA(const std::string &s) {
    size_t start= s.find('$');

    if(start == std::string::npos)
        throw NMEAFormatException("NMEA format corrupted! ($ not found)");

    std::string msg = s.substr(start);
    NMEA::cleanupMessage(msg);

    size_t fs = msg.find(',');
    size_t term = msg.length()-3;

    if(msg.length() < 4)
        throw NMEAFormatException("NMEA format corrupted! (Message too short)");
    else if(msg[0]!='$')
        throw NMEAFormatException("NMEA format corrupted! ($ not found)");
    else if(msg[term] != '*'){
        throw NMEAFormatException("NMEA format corrupted! (Terminator not found)");
    }else {
        this->checksum = msg.substr(term+1);
        if(NMEA::getChecksum(msg) != makeupper(this->checksum)) {
            std::string excMsg= "NMEA format corrupted! (Checksum invalid) ";
            excMsg+= NMEA::getChecksum(msg) + " != " + makeupper(this->checksum);

            throw NMEAFormatException(excMsg);
        }
    }

    if(fs == std::string::npos){
        this->name = msg.substr(1, term-1);
        this->data = NMEAData();
    } else {
        this->name = msg.substr(1, fs - 1);
        this->data = NMEAData(SuperMisc::split(msg.substr(fs + 1), ','));
    }
}

NMEA::NMEA(const std::string &name, const NMEAData &data) {
    this->name= name;
    this->data= data;

    std::string raw= name+","+data.toString();
    this->checksum= getChecksum(raw);
}

std::string NMEA::getChecksum(const std::string &msg) {
    std::string chars = std::string(msg);
    if(chars[0]=='$')
        chars = chars.substr(1);

    size_t term = chars.find('*');
    if(term != std::string::npos){
        chars = chars.substr(0, term);
    }

    int checksum = 0;
    for(char ch: chars)
        checksum ^= ch;

    char r[3];
    sprintf(r, "%02X", checksum);
    return std::string(r);
}

std::string &NMEA::makeupper(std::string &s) {
    for (auto & c: s) c = (char)toupper(c);
    return s;
}

std::string NMEA::getName() const {
    return name;
}

void NMEA::cleanupMessage(std::string &msg) {
    SuperMisc::removeChar(msg, '\n');
    SuperMisc::removeChar(msg, '\r');
}

std::string NMEA::getValue(int i) {
    return data.get(i);
}

std::string NMEA::toString() {
    std::string raw = "$" + this->name+","+this->data.toString();
    raw += "*" + this->checksum + "\n";

    return raw;
}

bool NMEA::isNamed(const std::string &n) const{
    return this->name==n;
}

int NMEA::countDataFields() {
    return data.size();
}
