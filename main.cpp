#include <iostream>
#include <fstream>
#include <vector>
#include "BytePacketBuffer.hpp"
#include "DnsPacket.hpp"
#include "UDPClient.hpp"

void readFile(std::string &fileName, BytePacketBuffer &buffer) {
    std::ifstream file(fileName, std::ios::binary);
    file.read((char *)&buffer.buf[0], sizeof(buffer.buf));
}

bool validateInput(std::string recordType, QueryType::QueryTypeEnum qtype) {
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

void performLookup(std::string &domain, std::string &servaddr, std::string &recordType) {
    QueryType::QueryTypeEnum qtype = QueryType::toEnum(std::stoi(recordType));
    if (!validateInput(recordType, qtype)) return;

    int servport = 53;  // DNS UDP port

    domain = domain + ".";

    UDPClient udp(servaddr, servport);
    DnsPacket packet;

    packet.header.pid = 1234;
    packet.header.questions = 1;
    packet.header.recursionDesired = true;
    DnsQuestion ques(domain, qtype);
    packet.questions.push_back(ques);

    BytePacketBuffer reqbuffer;
    packet.write(reqbuffer);
    std::cout << "Bytes sent: " << udp.send(reqbuffer) << std::endl;

    BytePacketBuffer resbuffer;
    std::cout << "Bytes received: " << udp.recv(resbuffer) << std::endl;

    DnsPacket respacket;
    respacket.packetFromBuffer(resbuffer);

    std::cout << respacket << std::endl;
}


int main(int argc, const char **argv) {
    if (argc != 4) {
        std::cout << "Usage: <executable-name> <lookup-domain> <record-type> <dns-resolver-ip>" << std::endl;
        return 0;
    }

    std::string domain = std::string(argv[1]);
    std::string servaddr = std::string(argv[3]);
    std::string recordType = std::string(argv[2]);
    performLookup(domain, servaddr, recordType);

    return 0;
}