//
// Created by dkulpa on 15.11.2022.
//

#include <iostream>
#include "BufferedReader.h"

BufferedReader::BufferedReader() {
    rxBuffHead= rxBuff;
    *rxBuffHead= 0;
}

int BufferedReader::update(char *rx) {
    int msgCnt = 0;

    // Check for buffer overflow
    if( ((rxBuffHead-rxBuff) + strlen(rx)) >= (2 * BUFFEREDREADER_MAX_BUFFER_SIZE) ){
        return -1; // OVERFLOW!
    }

    memcpy(rxBuffHead, rx, strlen(rx));
    rxBuffHead+= strlen(rx);
    *rxBuffHead= 0;


    char *endlPos = strchr(rxBuff, '\n');
    while(endlPos != nullptr){
        char msgBuff[BUFFEREDREADER_MAX_BUFFER_SIZE];
        memcpy(msgBuff, rxBuff, endlPos-rxBuff);
        msgBuff[endlPos-rxBuff] = 0;

        msgs.emplace_back(msgBuff);
        std::cout << "BR: New message: " << msgs[msgs.size()-1] << std::endl;

        msgCnt++;
        memmove(rxBuff, endlPos+1, rxBuffHead-endlPos);
        rxBuffHead-= (rxBuffHead-endlPos);
        endlPos = strchr(rxBuff, '\n');
    }

    return msgCnt;
}

std::string BufferedReader::popMsg() {
    return std::string();
}
