#include "UDPClient.hpp"
#include <unistd.h>


UDPClient::UDPClient(const std::string& addr, int port)
        : servPort(port)
        , servIP(addr)
{
    if ((udpSocket = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
        throw "could not create socket";
    }

    servAddr.sin_family = AF_INET;
    servAddr.sin_port = htons(servPort);
    servAddr.sin_addr.s_addr = inet_addr(servIP.c_str());

    servLen = sizeof(servAddr);
    clLen = sizeof(clAddr);
}

UDPClient::~UDPClient()
{
    close(udpSocket);
}

ssize_t UDPClient::send(const BytePacketBuffer &buffer) {
    return sendto(udpSocket, buffer.buf, sizeof(buffer.buf), 0, (struct sockaddr *)&servAddr, servLen);
}

ssize_t UDPClient::recv(BytePacketBuffer &buffer) {
    return recvfrom(udpSocket, buffer.buf, sizeof(buffer.buf), 0, (struct sockaddr *)&clAddr, &clLen);
}
