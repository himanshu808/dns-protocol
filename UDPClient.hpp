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
    UDPClient(const std::string& addr, int port);
    ~UDPClient();

    ssize_t send(const BytePacketBuffer &buffer);
    ssize_t recv(BytePacketBuffer &buffer);


private:
    int udpSocket;
    int servPort;
    std::string servIP;
    struct sockaddr_in servAddr, clAddr;
    socklen_t servLen, clLen;
};


#endif