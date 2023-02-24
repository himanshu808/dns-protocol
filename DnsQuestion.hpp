#ifndef DNSQUESTION_H
#define DNSQUESTION_H

#include <string>
#include <utility>
#include "BytePacketBuffer.hpp"
#include "Utils.hpp"

class QueryType {
private:
    static uint16_t qNum;

public:
    enum QueryTypeEnum: uint16_t {
        UNKNOWN = 0,
        A = 1,
        NS = 2,
        CNAME = 5,
        MX = 15,
        AAAA = 28
    };

    static QueryTypeEnum toEnum(uint16_t i) {
        switch (i) {
            case 1: return QueryTypeEnum::A;
            case 2: return QueryTypeEnum::NS;
            case 5: return QueryTypeEnum::CNAME;
            case 15: return QueryTypeEnum::MX;
            case 28: return QueryTypeEnum::AAAA;
            default: {
                qNum = i;
                return QueryTypeEnum::UNKNOWN;
            }
        }
    }

    static uint16_t toNum(QueryTypeEnum q) {
        if (q != QueryTypeEnum::UNKNOWN) return QueryTypeEnum(q);
        return qNum;
    }

    QueryTypeEnum getQType() {
        return qtype;
    }

private:
    QueryTypeEnum qtype;
};


class DnsQuestion {
private:
    std::string name;
    QueryType::QueryTypeEnum qType;

public:
    DnsQuestion(std::string n, QueryType::QueryTypeEnum qt) : name(std::move(n)), qType(qt) {}
    void read(BytePacketBuffer &buffer);
    friend std::ostream &operator<<(std::ostream &os, const DnsQuestion &question);
    void write(BytePacketBuffer &buffer) const;
};

#endif