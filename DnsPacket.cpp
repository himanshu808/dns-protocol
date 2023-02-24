#include "DnsPacket.hpp"


void DnsPacket::packetFromBuffer(BytePacketBuffer &buffer) {
    header.read(buffer);

    unsigned i;

    for (i = 0; i < header.questions; i++) {
        DnsQuestion question("", QueryType::QueryTypeEnum::UNKNOWN);
        question.read(buffer);
        questions.push_back(question);
    }

    for (i = 0; i < header.answers; i++) {
        DnsAnswer* answer;
        answer = DnsAnswer::read(buffer);
        answers.push_back(answer);
    }

    for (i = 0; i < header.authoritativeEntries; i++) {
        DnsAnswer* authAns;
        authAns = DnsAnswer::read(buffer);
        authorities.push_back(authAns);
    }

    for (i = 0; i < header.resourceEntries; i++) {
        DnsAnswer* resAns;
        resAns = DnsAnswer::read(buffer);
        resources.push_back(resAns);
    }

}

void DnsPacket::write(BytePacketBuffer &buffer) {
    header.questions = questions.size();
    header.answers = answers.size();
    header.authoritativeEntries = authorities.size();
    header.resourceEntries = resources.size();

    header.write(buffer);

    for (const auto & i : questions) i.write(buffer);
    for (const auto & i : answers) i->write(buffer);
    for (const auto & i : authorities) i->write(buffer);
    for (const auto & i : resources) i->write(buffer);
}

std::ostream &operator<<(std::ostream &os, const DnsPacket &packet) {
    os << packet.header << std::endl;

    for (const auto& q: packet.questions) os << q;
    os << std::endl;

    for (const auto& q: packet.answers) os << q;
    os << std::endl;

    for (const auto& q: packet.authorities) os << q;
    os << std::endl;

    for (const auto& q: packet.resources) os << q;
    os << std::endl;

    return os;
}
