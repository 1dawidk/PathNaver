//
// Created by dkulpa on 15.11.2022.
//

#include "BufferedReader.h"
#include "../Console.h"

BufferedReader::BufferedReader() {
    rxBuffHead= rxBuff;
    *rxBuffHead= 0;
    rxBuf= "";
}

int BufferedReader::update(char *rx) {
    int msgCnt = 0;

    rxBuf+= rx;
    size_t endlPos= rxBuf.find('\n');

    while (endlPos != std::string::npos){
        msgs.emplace_back(rxBuf.substr(0, endlPos));

        // Console::logi("BufferedReader", msgs[msgs.size()-1]);
        Console::logi("BufferedReader",
                      "New message ("+std::to_string(msgs[msgs.size()-1].length())+" chars)");
        if(msgs.size() > BUFFEREDREADER_MAX_QUEUE_SIZE){
            msgs.erase(msgs.begin());
        }

        msgCnt++;
        rxBuf.erase(0, endlPos+1);

        endlPos= rxBuf.find('\n');
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
