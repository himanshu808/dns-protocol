#include "DnsHeader.hpp"
#include <iostream>


void DnsHeader::read(BytePacketBuffer &buffer) {
    pid = buffer.read2Bytes();

    uint16_t flags = buffer.read2Bytes();
    uint8_t lFlags = flags >> 8;
    uint8_t rFlags = flags & 0xFF;

    recursionDesired = lFlags & 1;
    truncatedMsg = (lFlags & (1 << 1)) > 0;
    authoritativeAns = (lFlags & (1 << 2)) > 0;
    opcode = (lFlags >> 3) & 0x0F;
    queryResponse = (lFlags & (1 << 7)) > 0;

    responseCode = enumFromIdx<RCode>(rFlags & 0x0F);
    checkingDisabled = (rFlags & (1 << 4)) > 0;
    authedData = (rFlags & (1 << 5)) > 0;
    Z = (rFlags & (1 << 6)) > 0;
    recursionAvailable = (rFlags & (1 << 7)) > 0;

    questions = buffer.read2Bytes();
    answers = buffer.read2Bytes();
    authoritativeEntries = buffer.read2Bytes();
    resourceEntries = buffer.read2Bytes();
}

std::ostream &operator<<(std::ostream &os, const DnsHeader &header) {
    return os << "DnsHeader {" << std::endl
              << "\tpacket id: " << header.pid << std::endl
              << "\tqueryResponse: " << header.queryResponse << std::endl
              << "\topcode: " << (unsigned) header.opcode << std::endl
              << "\tauthoritativeAnswers: " << header.authoritativeAns << std::endl
              << "\ttruncatedMessage: " << header.truncatedMsg << std::endl
              << "\trecursionDesired: " << header.recursionDesired << std::endl
              << "\trecursionAvailable: " << header.recursionAvailable << std::endl
              << "\tZ: " << header.Z << std::endl
              << "\treponseCode: " << header.responseCode << std::endl
              << "\tcheckingDisabled: " << header.checkingDisabled << std::endl
              << "\tauthedData: " << header.authedData << std::endl
              << "\tquestions: " << header.questions << std::endl
              << "\tanswers: " << header.answers << std::endl
              << "\tauthoritativeEntries: " << header.authoritativeEntries << std::endl
              << "\tresourceEntries: " << header.resourceEntries << std::endl
              << "}" << std::endl;
}
