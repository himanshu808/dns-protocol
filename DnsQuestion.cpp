#include "DnsQuestion.hpp"
#include <iostream>


void DnsQuestion::read(BytePacketBuffer &buffer) {
    buffer.readDomainName(name);
    qType = enumFromIdx<QueryType>(buffer.read2Bytes());
    buffer.read2Bytes();
}

std::ostream &operator<<(std::ostream &os, const DnsQuestion &question) {
    return os << "DnsQuestion {" << std::endl
              << "\tname: " << question.name << std::endl
              << "\tqueryType: " << question.qType<< std::endl
              << "}" << std::endl;
}
