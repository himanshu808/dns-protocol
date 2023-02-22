#ifndef DNSANSWER_H
#define DNSANSWER_H

#include <unordered_map>
#include <cstdint>
#include "DnsQuestion.hpp"
#include "BytePacketBuffer.hpp"
#include "Utils.hpp"

class DnsAnswer {
public:
    static DnsAnswer* createDnsAns(std::string &domain, uint16_t queryType, uint16_t dataLen, uint32_t ttl);
    static DnsAnswer* createDnsAns(std::string &domain, std::string &addr , uint32_t ttl);
    static DnsAnswer* read(BytePacketBuffer &buffer);
    static void constructIPv4Addr(uint32_t rawIP, std::string &addr);
    virtual std::ostream& format (std::ostream &os) const { return os; }
    friend std::ostream& operator<<(std::ostream &os, const DnsAnswer *obj) { return obj->format(os); }
    static uint32_t deconstructIPv4Addr(const std::string &addr);
    virtual unsigned write(BytePacketBuffer &buffer) const=0;
};

class UnknownDnsAnswer : public DnsAnswer {
private:
    std::string domain;
    uint16_t queryType;
    uint16_t dataLen;
    uint32_t ttl;

public:
    UnknownDnsAnswer(std::string &domain, uint16_t queryType, uint16_t dataLen, uint32_t ttl)
        : domain(domain)
        , queryType(queryType)
        , dataLen(dataLen)
        , ttl(ttl) {}

//    friend std::ostream& operator<<(std::ostream &os, const UnknownDnsAnswer &answer);
    std::ostream& format(std::ostream &os) const override;
    unsigned write(BytePacketBuffer &buffer) const override;
};

class ATypeDnsAnswer : public DnsAnswer {
private:
    std::string domain;
    std::string addr;
    uint32_t ttl;

public:
    ATypeDnsAnswer(std::string &domain, std::string &addr, uint32_t ttl)
        : domain(domain)
        , addr(addr)
        , ttl(ttl) {}

//    friend std::ostream& operator<<(std::ostream &os, const ATypeDnsAnswer &answer);
    std::ostream& format(std::ostream &os) const override;
    unsigned write(BytePacketBuffer &buffer) const override;
};

#endif
