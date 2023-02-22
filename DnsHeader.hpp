#ifndef DNSHEADER_H
#define DNSHEADER_H

#include "BytePacketBuffer.hpp"
#include "Utils.hpp"
#include <cstdint>


enum RCode {
    NOERROR,    // 0
    FORMERR,    // 1
    SERVFAIL,   // 2
    NXDOMAIN,   // 3
    NOTIMP,     // 4
    REFUSED     // 5
};



class DnsHeader {
public:
    uint16_t pid;
    bool queryResponse;
    uint8_t opcode;
    bool authoritativeAns;
    bool truncatedMsg;
    bool recursionDesired;
    bool recursionAvailable;
    bool Z;
    RCode responseCode;
    bool checkingDisabled;
    bool authedData;
    uint16_t questions;
    uint16_t answers;
    uint16_t authoritativeEntries;
    uint16_t resourceEntries;

    DnsHeader()
        : pid(0)
        , recursionDesired (false)
        , truncatedMsg(false)
        , authoritativeAns(false)
        , opcode(0)
        , queryResponse(false)
        , responseCode(RCode::NOERROR)
        , checkingDisabled(false)
        , authedData(false)
        , Z(false)
        , recursionAvailable(false)
        , questions(0)
        , answers(0)
        , authoritativeEntries(0)
        , resourceEntries(0) {}

    void read(BytePacketBuffer &buffer);
    void write(BytePacketBuffer &buffer) const;

    friend std::ostream &operator<<(std::ostream &os, const DnsHeader &header);
};

#endif