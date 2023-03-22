#ifndef DNSPACKET_H
#define DNSPACKET_H

#include "DnsHeader.hpp"
#include "DnsQuestion.hpp"
#include "DnsAnswer.hpp"
#include "BytePacketBuffer.hpp"
#include "Utils.hpp"
#include <vector>
#include <iostream>

class DnsPacket {
public:
    DnsHeader header;
    std::vector<DnsQuestion> questions;
    std::vector<DnsAnswer*> answers;
    std::vector<DnsAnswer*> resources;
    std::vector<DnsAnswer*> authorities;

    void packetFromBuffer(BytePacketBuffer &buffer);
    void write(BytePacketBuffer &buffer);

    std::string getRandomARecord();
    std::vector<std::pair<std::string, std::string>> getAllNameServers(const std::string &domainName);
    std::string getResolvedNS(const std::string &domain);
    std::string getUnresolvedNS(const std::string &domain);

    friend std::ostream& operator<< (std::ostream& os, const DnsPacket &packet);
};

#endif