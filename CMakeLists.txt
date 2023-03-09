cmake_minimum_required(VERSION 3.23)
project(dns_protocol)

set(CMAKE_CXX_STANDARD 17)

include_directories(.)

add_executable(
        dns_protocol
        main.cpp
        BytePacketBuffer.cpp
        UDPClient.cpp
        DnsHeader.cpp
        DnsQuestion.cpp
        DnsAnswer.cpp
        DnsPacket.cpp
        DNSProxy.cpp
)
