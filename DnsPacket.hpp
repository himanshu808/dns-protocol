#ifndef DNSPACKET_H
#define DNSPACKET_H

#include "DnsHeader.hpp"
#include "DnsQuestion.hpp"
#include "DnsAnswer.hpp"
#include "BytePacketBuffer.hpp"
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

    friend std::ostream& operator<< (std::ostream& os, const DnsPacket &packet);
};

#endif