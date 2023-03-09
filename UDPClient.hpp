#ifndef UDP_CLIENT_H
#define UDP_CLIENT_H

#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <stdexcept>
#include <string>
#include <arpa/inet.h>
#include "BytePacketBuffer.hpp"


class UDPClient {
public:
    UDPClient();
    ~UDPClient();

    ssize_t send(const BytePacketBuffer &buffer, std::string &sendToAddr, int sendToPort);
    std::pair<ssize_t, std::pair<std::string, int>> recv(BytePacketBuffer &buffer);
    void bind(std::string &addr, int port);


private:
    int udpSocket;
};


#endif