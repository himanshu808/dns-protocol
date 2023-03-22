#ifndef DNSPROXY_H
#define DNSPROXY_H

#include <string>
#include <iostream>
#include "DnsQuestion.hpp"
#include "UDPClient.hpp"
#include "DnsPacket.hpp"
#include "BytePacketBuffer.hpp"

class DNSProxy {
private:
    bool validateInput(std::string &recordType, QueryType::QueryTypeEnum qtype);
    DnsPacket performLookup(std::string &domain, QueryType::QueryTypeEnum qtype, std::string &serverIP);
    DnsPacket recursiveResolve(std::string &domain, QueryType::QueryTypeEnum qtype);

public:
    void handleRequest(UDPClient &udpSocket);
};

#endif