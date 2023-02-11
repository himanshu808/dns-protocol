#ifndef DNSQUESTION_H
#define DNSQUESTION_H

#include <string>
#include <utility>
#include "BytePacketBuffer.hpp"
#include "Utils.hpp"


enum QueryType {
    UNKNOWN,    // 0
    A           // 1
};


class DnsQuestion {
private:
    std::string name;
    QueryType qType;

public:
    DnsQuestion(std::string n, QueryType qt) : name(std::move(n)), qType(qt) {}
    void read(BytePacketBuffer &buffer);
    friend std::ostream &operator<<(std::ostream &os, const DnsQuestion &question);
};

#endif