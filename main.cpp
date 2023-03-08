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


int main(int argc, const char **argv) {
    if (argc != 4) {
        std::cout << "Usage: <executable-name> <lookup-domain> <record-type> <dns-resolver-ip>" << std::endl;
        return 0;
    } else {
        std::string recordType = std::string(argv[2]);
        if (!std::all_of(recordType.begin(), recordType.end(), ::isdigit)) {
            std::cout << "<record-type> should only contain digits" << std::endl;
            return 0;
        }
    };


    QueryType::QueryTypeEnum qtype = QueryType::toEnum(std::stoi(argv[2]));
    if (qtype == QueryType::QueryTypeEnum::UNKNOWN) {
        std::cout << "<record-type> can be either 1, 2, 5, 15, 28" << std::endl;
        return 0;
    }
    std::string domain = std::string(argv[1]) + ".";

    std::string servaddr = std::string(argv[3]);
    int servport = 53;  // DNS UDP port

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

    return 0;
}