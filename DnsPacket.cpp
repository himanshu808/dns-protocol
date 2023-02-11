#include "DnsPacket.hpp"


void DnsPacket::packetFromBuffer(BytePacketBuffer &buffer) {
    header.read(buffer);

    unsigned i;
    DnsAnswer* answer;

    for (i = 0; i < header.questions; i++) {
        DnsQuestion question("", QueryType::UNKNOWN);
        question.read(buffer);
        questions.push_back(question);
    }

    for (i = 0; i < header.answers; i++) {
        answer = DnsAnswer::read(buffer);
        answers.push_back(answer);
    }

    for (i = 0; i < header.authoritativeEntries; i++) {
        answer = DnsAnswer::read(buffer);
        authorities.push_back(answer);
    }

    for (i = 0; i < header.resourceEntries; i++) {
        answer = DnsAnswer::read(buffer);
        resources.push_back(answer);
    }

}