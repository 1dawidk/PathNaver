//
// Created by dkulpa on 18.11.2022.
//

#ifndef PATHNAVER_NMEADATA_H
#define PATHNAVER_NMEADATA_H

#include <vector>
#include <string>
#include <queue>

class NMEAData {
public:
    NMEAData();
    explicit NMEAData(std::vector<std::string> d);

    void add(const std::string& v);
    void add(unsigned int v);
    void add(int v);
    void add(unsigned char v);
    void add(char v);
    void add(unsigned long v);
    void add(long v);
    void add(bool v);
    void add(float v);
    void add(double v);

    std::string& get(int i);

    [[nodiscard]] std::string toString() const;
private:
    std::vector<std::string> data;
};


#endif //PATHNAVER_NMEADATA_H
