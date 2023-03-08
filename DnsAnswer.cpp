#include "DnsAnswer.hpp"
#include <vector>
#include <iostream>
#include<sstream>

void DnsAnswer::constructIPv4Addr(uint32_t rawIP, std::string &addr) {
    for (int i = 3; i >= 0 ; i--) {
        addr += std::to_string(static_cast<uint8_t>((rawIP >> (8*i)) & 0xFF)) + ".";
    }
    if (!addr.empty()) addr.pop_back();
}

void DnsAnswer::constructIPv6Addr(__uint128_t rawIP, std::string &addr) {
    std::stringstream hexstream;

    for (int i = 7; i >= 0 ; i--) {
        hexstream << std::hex << static_cast<uint16_t>((rawIP >> (16*i)) & 0xFFFF);
        hexstream << ":";
    }
    addr = hexstream.str();
    if (!addr.empty()) addr.pop_back();
}

DnsAnswer* DnsAnswer::read(BytePacketBuffer &buffer) {
    std::string domain, addr;
    buffer.readDomainName(domain);

    auto queryType = QueryType::QueryTypeEnum(buffer.read2Bytes());
    buffer.read2Bytes();  // skip class
    uint32_t ttl = buffer.read4Bytes();
    uint16_t dataLen = buffer.read2Bytes();

    switch (queryType) {
        case QueryType::QueryTypeEnum::A : {
            uint32_t rawIp;
            rawIp = buffer.read4Bytes();
            DnsAnswer::constructIPv4Addr(rawIp, addr);
            return createDnsAns(domain, addr, ttl, QueryType::QueryTypeEnum::A);
        }
        case QueryType::QueryTypeEnum::AAAA : {
            __int128_t rawIp;
            for (int i = 7; i >= 0; i--){
                rawIp = (rawIp << 16) | buffer.read2Bytes();
            }
            DnsAnswer::constructIPv6Addr(rawIp, addr);
            return createDnsAns(domain, addr, ttl, QueryType::QueryTypeEnum::AAAA);
        }
        case QueryType::QueryTypeEnum::NS : {
            buffer.readDomainName(addr);
            return createDnsAns(domain, addr, ttl, QueryType::QueryTypeEnum::NS);
        }
        case QueryType::QueryTypeEnum::CNAME : {
            buffer.readDomainName(addr);
            return createDnsAns(domain, addr, ttl, QueryType::QueryTypeEnum::CNAME);
        }
        case QueryType::QueryTypeEnum::MX : {
            uint16_t priority = buffer.read2Bytes();
            buffer.readDomainName(addr);
            return createDnsAns(domain, priority, addr, ttl);
        }
        default : {
            buffer.step(dataLen);
            return createDnsAns(domain, queryType, dataLen, ttl);
        }
    }
}

DnsAnswer* DnsAnswer::createDnsAns(std::string &domain, uint16_t queryType, uint16_t dataLen, uint32_t ttl) {
    DnsAnswer* ans = new UnknownDnsAnswer(domain, queryType, dataLen, ttl);
    return ans;
}

DnsAnswer *DnsAnswer::createDnsAns(std::string &domain, uint16_t priority, std::string &host, uint32_t ttl) {
    DnsAnswer* ans = new MXTypeDnsAnswer(domain, priority, host, ttl);
    return ans;
}

DnsAnswer* DnsAnswer::createDnsAns(std::string &domain, std::string &addr , uint32_t ttl,
                                   QueryType::QueryTypeEnum qtype) {
    DnsAnswer* ans;
    if (qtype == QueryType::QueryTypeEnum::A) {
        ans = new ATypeDnsAnswer(domain, addr, ttl);
    } else if (qtype == QueryType::QueryTypeEnum::AAAA) {
        ans = new AAAATypeDnsAnswer(domain, addr, ttl);
    } else if (qtype == QueryType::QueryTypeEnum::NS) {
        ans = new NSTypeDnsAnswer(domain, addr, ttl);
    } else {  // CNAME
        ans = new CNameTypeDnsAnswer(domain, addr, ttl);
    }
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

__uint128_t DnsAnswer::deconstructIPv6Addr(const std::string &addr) {
    char delimiter = ':';
    size_t prev = 0;
    size_t next = 0;
    __uint128_t res = 0;
    unsigned i = 0;

    while ((next = addr.find(delimiter,prev)) != std::string::npos) {
        res = (res << (16 * i)) | static_cast<uint16_t>(strtol(addr.substr(prev, next-prev).c_str(),
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

    unsigned startPos = buffer.getPos();

    buffer.writeDomainName(domain);
    buffer.write2Bytes(QueryType::toNum(QueryType::A));
    buffer.write2Bytes(1);  // class is always set to 1
    buffer.write4Bytes(ttl);
    buffer.write2Bytes(4);  // setting data len to 4; because of ip addr?

    ip = DnsAnswer::deconstructIPv4Addr(addr);
    buffer.write4Bytes(ip );  // TODO: should work instead of 4 - 1 byte calls

    return buffer.getPos() - startPos;
}

std::ostream &NSTypeDnsAnswer::format(std::ostream &os) const {
    return os << "NSTypeDnsAnswer {" << std::endl
              << "\tdomain: " << domain << std::endl
              << "\thost: " << host << std::endl
              << "\tttl: " << ttl << std::endl
              << "}" << std::endl;
}

unsigned NSTypeDnsAnswer::write(BytePacketBuffer &buffer) const {
    unsigned startPos = buffer.getPos();

    buffer.writeDomainName(domain);
    buffer.write2Bytes(QueryType::toNum(QueryType::NS));
    buffer.write2Bytes(1);  // class is always set to 1
    buffer.write4Bytes(ttl);

    unsigned currPos = buffer.getPos();
    buffer.write2Bytes(0);
    buffer.writeDomainName(host);
    buffer.set2Bytes(currPos, buffer.getPos() - (currPos+2));

    return buffer.getPos() - startPos;
}

std::ostream &CNameTypeDnsAnswer::format(std::ostream &os) const {
    return os << "CNameTypeDnsAnswer {" << std::endl
              << "\tdomain: " << domain << std::endl
              << "\thost: " << host << std::endl
              << "\tttl: " << ttl << std::endl
              << "}" << std::endl;
}

unsigned CNameTypeDnsAnswer::write(BytePacketBuffer &buffer) const {
    unsigned startPos = buffer.getPos();

    buffer.writeDomainName(domain);
    buffer.write2Bytes(QueryType::toNum(QueryType::CNAME));
    buffer.write2Bytes(1);  // class is always set to 1
    buffer.write4Bytes(ttl);

    unsigned currPos = buffer.getPos();
    buffer.write2Bytes(0);
    buffer.writeDomainName(host);
    buffer.set2Bytes(currPos, buffer.getPos() - (currPos+2));

    return buffer.getPos() - startPos;
}

std::ostream &MXTypeDnsAnswer::format(std::ostream &os) const {
    return os << "MXTypeDnsAnswer {" << std::endl
              << "\tdomain: " << domain << std::endl
              << "\tpriority: " << priority << std::endl
              << "\thost: " << host << std::endl
              << "\tttl: " << ttl << std::endl
              << "}" << std::endl;
}

unsigned MXTypeDnsAnswer::write(BytePacketBuffer &buffer) const {
    unsigned startPos = buffer.getPos();

    buffer.writeDomainName(domain);
    buffer.write2Bytes(QueryType::toNum(QueryType::MX));
    buffer.write2Bytes(1);  // class is always set to 1
    buffer.write4Bytes(ttl);

    unsigned currPos = buffer.getPos();
    buffer.write2Bytes(0);

    buffer.write2Bytes(priority);
    buffer.writeDomainName(host);

    buffer.set2Bytes(currPos, buffer.getPos() - (currPos+2));

    return buffer.getPos() - startPos;
}

std::ostream &AAAATypeDnsAnswer::format(std::ostream &os) const {
    return os << "AAAATypeDnsAnswer {" << std::endl
              << "\tdomain: " << domain << std::endl
              << "\taddr: " << ipv6addr << std::endl
              << "\tttl: " << ttl << std::endl
              << "}" << std::endl;
}

unsigned AAAATypeDnsAnswer::write(BytePacketBuffer &buffer) const {
    __int128_t ipv6;

    unsigned startPos = buffer.getPos();

    buffer.writeDomainName(domain);
    buffer.write2Bytes(QueryType::toNum(QueryType::AAAA));
    buffer.write2Bytes(1);  // class is always set to 1
    buffer.write4Bytes(ttl);
    buffer.write2Bytes(16);  // because of ipv6?

    ipv6 = deconstructIPv6Addr(ipv6addr);
    for (int i = 7; i >= 0; i--) {
        buffer.write2Bytes(static_cast<uint16_t>((ipv6 >> (16*i)) & 0xFFFF));
    }

    return buffer.getPos() - startPos;
}
