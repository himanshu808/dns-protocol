#include <iostream>
#include <fstream>
#include <vector>
#include "BytePacketBuffer.hpp"
#include "DnsPacket.hpp"
#include "UDPClient.hpp"
#include "DNSProxy.hpp"


int main(int argc, const char **argv) {
    if (argc != 2) {
        std::cout << "Usage: <executable-name> <binding-port-num>" << std::endl;
        return 0;
    }

    std::string addr = "0.0.0.0";
    int port = std::stoi(argv[1]);
    UDPClient udpClient;
    udpClient.bind(addr, port);
    DNSProxy proxy;

    std::cout << "Proxy server listening on " << port << "..." << std::endl;
    do {
        try{
            proxy.handleRequest(udpClient);
        } catch (std::exception &e) {}
    } while(true);

}