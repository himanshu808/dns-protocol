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
    static DnsAnswer* createDnsAns(std::string &domain, uint16_t priority, std::string &host, uint32_t ttl);
    static DnsAnswer* createDnsAns(std::string &domain, std::string &addr , uint32_t ttl,
                                   QueryType::QueryTypeEnum qtype);
    static DnsAnswer* read(BytePacketBuffer &buffer);
    static void constructIPv4Addr(uint32_t rawIP, std::string &addr);
    virtual std::ostream& format (std::ostream &os) const { return os; }
    friend std::ostream& operator<<(std::ostream &os, const DnsAnswer *obj) { return obj->format(os); }
    static uint32_t deconstructIPv4Addr(const std::string &addr);
    virtual unsigned write(BytePacketBuffer &buffer) const=0;
    static void constructIPv6Addr(__uint128_t rawIP, std::string &addr);
    static __uint128_t deconstructIPv6Addr(const std::string &addr);
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
    [[nodiscard]] std::string getIPAddr() const { return addr; }
    [[nodiscard]] std::string getDomain() const { return domain; }
};

class NSTypeDnsAnswer : public DnsAnswer {
private:
    std::string domain;
    std::string host;
    uint32_t ttl;

public:
    NSTypeDnsAnswer(std::string &domain, std::string &host, uint32_t ttl)
            : domain(domain)
            , host(host)
            , ttl(ttl) {}

    std::ostream& format(std::ostream &os) const override;
    unsigned write(BytePacketBuffer &buffer) const override;
    [[nodiscard]] std::string getDomain() const { return domain; }
    [[nodiscard]] std::string getHost() const { return host; }
};

class CNameTypeDnsAnswer : public DnsAnswer {
private:
    std::string domain;
    std::string host;
    uint32_t ttl;

public:
    CNameTypeDnsAnswer(std::string &domain, std::string &host, uint32_t ttl)
            : domain(domain)
            , host(host)
            , ttl(ttl) {}

    std::ostream& format(std::ostream &os) const override;
    unsigned write(BytePacketBuffer &buffer) const override;
};

class MXTypeDnsAnswer : public DnsAnswer {
private:
    std::string domain;
    uint16_t priority;
    std::string host;
    uint32_t ttl;

public:
    MXTypeDnsAnswer(std::string &domain, uint16_t priority, std::string &host, uint32_t ttl)
            : domain(domain)
            , priority(priority)
            , host(host)
            , ttl(ttl) {}

    std::ostream& format(std::ostream &os) const override;
    unsigned write(BytePacketBuffer &buffer) const override;
};

class AAAATypeDnsAnswer : public DnsAnswer {
private:
    std::string domain;
    std::string ipv6addr;
    uint32_t ttl;

public:
    AAAATypeDnsAnswer(std::string &domain, std::string &addr, uint32_t ttl)
            : domain(domain)
            , ipv6addr(addr)
            , ttl(ttl) {}

    std::ostream& format(std::ostream &os) const override;
    unsigned write(BytePacketBuffer &buffer) const override;
};

#endif
