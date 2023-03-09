#include "UDPClient.hpp"
#include <unistd.h>
#include <iostream>


UDPClient::UDPClient()
{
    if ((udpSocket = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
        throw "could not create socket";
    }

}

UDPClient::~UDPClient()
{
    close(udpSocket);
}

ssize_t UDPClient::send(const BytePacketBuffer &buffer, std::string &sendToAddr, int sendToPort) {
    struct sockaddr_in sendAddr;
    sendAddr.sin_family = AF_INET;
    sendAddr.sin_port = htons(sendToPort);
    sendAddr.sin_addr.s_addr = inet_addr(sendToAddr.c_str());

    return sendto(udpSocket, buffer.buf, sizeof(buffer.buf), 0, (struct sockaddr *)&sendAddr, sizeof(sendAddr));
}

std::pair<ssize_t, std::pair<std::string, int>> UDPClient::recv(BytePacketBuffer &buffer) {
    struct sockaddr_in clAddr;
    socklen_t clLen;
    char addr[INET_ADDRSTRLEN];
    clLen = sizeof(clAddr);
    ssize_t s = recvfrom(udpSocket, buffer.buf, sizeof(buffer.buf), 0, (struct sockaddr *)&clAddr, &clLen);
    inet_ntop(AF_INET, &(clAddr.sin_addr), addr, INET_ADDRSTRLEN);
    return std::make_pair(s, std::make_pair(std::string(addr), ntohs(clAddr.sin_port)));
}

void UDPClient::bind(std::string &addr, int port) {
    struct sockaddr_in bindAddr;
    bindAddr.sin_family = AF_INET;
    bindAddr.sin_port = htons(port);
    bindAddr.sin_addr.s_addr = inet_addr(addr.c_str());

    if (::bind(udpSocket, (struct sockaddr *)&bindAddr, sizeof(bindAddr)) < 0) {
        std::cout << "Error binding socket to port: " << port << std::endl;
    }
}
