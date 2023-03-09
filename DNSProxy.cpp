#include "DNSProxy.hpp"

bool DNSProxy::validateInput(std::string &recordType, QueryType::QueryTypeEnum qtype) {
    if (!std::all_of(recordType.begin(), recordType.end(), ::isdigit)) {
        std::cout << "<record-type> should only contain digits" << std::endl;
        return false;
    }

    if (qtype == QueryType::QueryTypeEnum::UNKNOWN) {
        std::cout << "<record-type> can be either 1, 2, 5, 15, 28" << std::endl;
        return false;
    }

    return true;
}

DnsPacket DNSProxy::performLookup(std::string &domain, QueryType::QueryTypeEnum qtype) {
    int servport = 53;  // DNS UDP port
    std::string servaddr = "8.8.8.8";

    UDPClient udp;
    std::string bindAddr = "0.0.0.0";
    udp.bind(bindAddr, 8989);
    DnsPacket packet;

    packet.header.pid = 1234 ;
    packet.header.questions = 1;
    packet.header.recursionDesired = true;
    DnsQuestion ques(domain, qtype);
    packet.questions.push_back(ques);

    std::cout << "Fowarding proxy packet to DNS resolver...\n" << packet << std::endl;

    BytePacketBuffer reqbuffer;
    packet.write(reqbuffer);
    std::cout << "Bytes sent: " << udp.send(reqbuffer, servaddr, servport) << std::endl;

    BytePacketBuffer resbuffer;

    std::pair<ssize_t, std::pair<std::string, int>> source = udp.recv(resbuffer);
    std::cout << "Bytes received: " << source.first << std::endl;

    DnsPacket respacket;
    respacket.packetFromBuffer(resbuffer);

    std::cout << "Response recvd from DNS resolver...\n" << respacket << std::endl;
    return respacket;
}

void DNSProxy::handleRequest(UDPClient &udpSocket) {
    BytePacketBuffer requestBuffer;
    std::pair<ssize_t, std::pair<std::string, int>> source = udpSocket.recv(requestBuffer);

    DnsPacket requestPacket;
    requestPacket.packetFromBuffer(requestBuffer);

    std::cout << "Recvd packet from client...\n" << requestPacket << std::endl;

    DnsPacket proxyPacket;
    proxyPacket.header.pid = requestPacket.header.pid;
    proxyPacket.header.recursionDesired = true;
    proxyPacket.header.recursionAvailable = true;
    proxyPacket.header.queryResponse = true;

    if (requestPacket.questions.size() == 1) {
        // there should be exactly one question present
        DnsQuestion question = requestPacket.questions.front();
        requestPacket.questions.pop_back();
        std::cout << "Client query: " << question << std::endl;

        std::string domain = question.getDomainName();
        DnsPacket lookupPacket = performLookup(domain, question.getQueryType());

        if (lookupPacket.header.responseCode == RCode::NOERROR) {
            proxyPacket.questions.push_back(question);
            proxyPacket.header.responseCode = lookupPacket.header.responseCode;

            for (const auto& q: lookupPacket.answers) {
//                std::cout << "Answer: " << q << std::endl;
                proxyPacket.answers.push_back(q);
            }

            for (const auto& q: lookupPacket.authorities) {
//                std::cout << "Authority: " << q << std::endl;
                proxyPacket.authorities.push_back(q);
            }

            for (const auto& q: lookupPacket.resources) {
//                std::cout << "Resource: " << q << std::endl;
                proxyPacket.resources.push_back(q);
            }
        } else {
            proxyPacket.header.responseCode = RCode::SERVFAIL;
        }

    } else {
        // else the packet is malformed
        proxyPacket.header.responseCode = RCode::FORMERR;
    }

    BytePacketBuffer responseBuffer;
    proxyPacket.write(responseBuffer);

    std::cout << "Forwarding response to client...\nBytes sent: " << udpSocket.send(responseBuffer,
                                                                                    source.second.first,
                                                                                    source.second.second)
                                                                                    << std::endl;
}


