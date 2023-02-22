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
    std::string domain = "google.com.";
    QueryType qtype = QueryType::A;

    std::string servaddr = "8.8.8.8";
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