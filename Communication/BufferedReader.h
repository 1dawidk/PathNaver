//
// Created by dkulpa on 15.11.2022.
//

#ifndef PATHNAVER_BUFFEREDREADER_H
#define PATHNAVER_BUFFEREDREADER_H

#include <vector>
#include <string>
#include "cstring"

#define BUFFEREDREADER_MAX_BUFFER_SIZE  1024

class BufferedReader {
public:
    BufferedReader();
    int update(char *rx);
    std::string popMsg();
private:
    char rxBuff[2*BUFFEREDREADER_MAX_BUFFER_SIZE]{};
    char *rxBuffHead;

    std::vector<std::string> msgs;
};


#endif //PATHNAVER_BUFFEREDREADER_H
