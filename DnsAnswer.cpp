#include "DnsAnswer.hpp"
#include <vector>
#include <iostream>

void DnsAnswer::constructIPv4Addr(uint32_t rawIP, std::string &addr) {
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
            DnsAnswer::constructIPv4Addr(rawIp, addr);
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

uint32_t DnsAnswer::deconstructIPv4Addr(const std::string &addr) {
    char delimiter = '.';
    size_t prev = 0;
    size_t next = 0;
    uint32_t res = 0;
    unsigned i = 0;

    while ((next = addr.find(delimiter,prev)) != std::string::npos) {
        res = (res << (8 * i)) | static_cast<uint8_t>(strtol(addr.substr(prev, next-prev).c_str(),
                                                             nullptr, 10));
        i++;
        prev = next + 1;
    }
    return res;
}

unsigned UnknownDnsAnswer::write(BytePacketBuffer &buffer) const {
    std::cout << "Skipping record ..." << std::endl;
    std::cout << this << std::endl;
    return 0;
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

unsigned ATypeDnsAnswer::write(BytePacketBuffer &buffer) const {
    uint32_t ip;

    unsigned start_pos = buffer.getPos();

    buffer.writeDomainName(domain);
    buffer.write2Bytes(idxFromEnum<QueryType>(QueryType::A));
    buffer.write2Bytes(1);  // class is always set to 1
    buffer.write4Bytes(ttl);
    buffer.write2Bytes(4);  // setting data len to 4; because of ip addr?

    ip = DnsAnswer::deconstructIPv4Addr(addr);
    buffer.write4Bytes(ip );  // TODO: should work instead of 4 - 1 byte calls

    return buffer.getPos() - start_pos;
}
