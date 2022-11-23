//
// Created by dkulpa on 15.11.2022.
//

#include "BufferedReader.h"
#include "../Console.h"

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
        Console::logi("BufferedReader", msgBuff);
        if(msgs.size() > BUFFEREDREADER_MAX_QUEUE_SIZE){
            msgs.erase(msgs.begin());
        }

        msgCnt++;
        memmove(rxBuff, endlPos+1, endlPos-rxBuff);
        rxBuffHead-= (endlPos-rxBuff+1);
        endlPos = strchr(rxBuff, '\n');
    }

    return msgCnt;
}

std::string BufferedReader::popMsg() {
    std::string msg;

    if(!msgs.empty()) {
        msg = msgs[0];
        msgs.erase(msgs.begin());
    }

    return msg;
}

int BufferedReader::queueSize() {
    return static_cast<int>(msgs.size());
}
