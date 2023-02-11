#ifndef DNSPACKET_H
#define DNSPACKET_H

#include "DnsHeader.hpp"
#include "DnsQuestion.hpp"
#include "DnsAnswer.hpp"
#include "BytePacketBuffer.hpp"
#include <vector>

class DnsPacket {
public:
    DnsHeader header;
    std::vector<DnsQuestion> questions;
    std::vector<DnsAnswer*> answers;
    std::vector<DnsAnswer*> resources;
    std::vector<DnsAnswer*> authorities;

//    DnsPacket() {
//        questions = std::vector<DnsQuestion>();
//        answers = std::vector<DnsAnswer>();
//        resources = std::vector<DnsAnswer>();
//        authorities = std::vector<DnsAnswer>();
//    }
    void packetFromBuffer(BytePacketBuffer &buffer);
};

#endif