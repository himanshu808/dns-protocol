#include <iostream>
#include <fstream>
#include <vector>
#include "BytePacketBuffer.hpp"
#include "DnsPacket.hpp"

void readFile(std::string &fileName, BytePacketBuffer &buffer) {
    std::ifstream file(fileName, std::ios::binary);
    file.read((char *)&buffer.buf[0], sizeof(buffer.buf));
}


int main(int argc, const char **argv) {

    if (argc != 2) {
        std::cout << "Usage: <executable-name> <path-do-dns-resp-packet>";
        return 0;
    }

    BytePacketBuffer buffer;
    std::string fileName = argv[1];
    readFile(fileName, buffer);

    DnsPacket packet;
    packet.packetFromBuffer(buffer);
    std::cout << packet.header << std::endl;

    for (const auto& q: packet.questions) std::cout << q;
    std::cout << std::endl;

    for (const auto& q: packet.answers) std::cout << q;
    std::cout << std::endl;

    for (const auto& q: packet.authorities) std::cout << q;
    std::cout << std::endl;

    for (const auto& q: packet.resources) std::cout << q;
    std::cout << std::endl;

    return 0;
}