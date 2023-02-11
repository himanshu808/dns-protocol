#include "DnsAnswer.hpp"
#include <vector>
#include <iostream>

void DnsAnswer::constructIPAddr(uint32_t rawIP, std::string &addr) {
    for (int i = 3; i >= 0 ; i--) {
        addr += std::to_string(static_cast<uint8_t>((rawIP >> (8*i)) & 0xFF)) + ".";
    }
    if (!addr.empty()) addr.pop_back();
}

DnsAnswer* DnsAnswer::read(BytePacketBuffer &buffer) {
    std::string domain, addr;
    uint32_t rawIp;
    buffer.readDomainName(domain);

    auto queryType = enumFromIdx<QueryType>(buffer.read2Bytes());
    buffer.read2Bytes();  // skip class
    uint32_t ttl = buffer.read4Bytes();
    uint16_t dataLen = buffer.read2Bytes();

    switch (queryType) {
        case QueryType::A : {
            rawIp = buffer.read4Bytes();
            DnsAnswer::constructIPAddr(rawIp, addr);
            return createDnsAns(domain, addr, ttl);
        }
        case QueryType::UNKNOWN : {
            buffer.step(dataLen);
            return createDnsAns(domain, queryType, dataLen, ttl);
        }
    }
}

DnsAnswer* DnsAnswer::createDnsAns(std::string &domain, uint16_t queryType, uint16_t dataLen, uint32_t ttl) {
    DnsAnswer* ans = new UnknownDnsAnswer(domain, queryType, dataLen, ttl);
    return ans;
}

DnsAnswer* DnsAnswer::createDnsAns(std::string &domain, std::string &addr , uint32_t ttl) {
    DnsAnswer* ans = new ATypeDnsAnswer(domain, addr, ttl);
    return ans;
}


std::ostream& UnknownDnsAnswer::format(std::ostream &os) const {
    return os << "UnknownDnsAnswer {" << std::endl
              << "\tdomain: " << domain << std::endl
              << "\tqueryType: " << queryType << std::endl
              << "\tttl: " << ttl << std::endl
              << "}" << std::endl;
}

std::ostream& ATypeDnsAnswer::format(std::ostream &os) const {
    return os << "ATypeDnsAnswer {" << std::endl
              << "\tdomain: " << domain << std::endl
              << "\taddr: " << addr << std::endl
              << "\tttl: " << ttl << std::endl
              << "}" << std::endl;
}