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

std::string DnsPacket::getRandomARecord() {
    for (const auto& q: answers) {
        auto *ans = dynamic_cast<ATypeDnsAnswer*>(q);
        if (ans == nullptr) continue;
        else {
            return ans->getIPAddr();
        }
    }
    return "";
}

std::vector<std::pair<std::string, std::string>> DnsPacket::getAllNameServers(const std::string &domainName) {
    std::vector<std::pair<std::string, std::string>> ns;
    for (const auto& q: authorities) {
        auto *ans = dynamic_cast<NSTypeDnsAnswer*>(q);
        if (ans == nullptr) continue;
        else {
            std::string domain = ans->getDomain();
            if (!hasEnding(domainName, domain)) continue;
            ns.emplace_back(domain, ans->getHost());
        }
    }
    return ns;
}

std::string DnsPacket::getResolvedNS(const std::string &domain) {
    std::vector<std::pair<std::string, std::string>> nameServers = getAllNameServers(domain);
    for (const auto& res: resources) {
        auto *ans = dynamic_cast<ATypeDnsAnswer*>(res);
        if (ans == nullptr) continue;
        std::string domainName = ans->getDomain();
        for (const auto& ns: nameServers) {
            if (domainName == ns.second) {
                return ans->getIPAddr();
            }
        }
    }
    return "";
}

std::string DnsPacket::getUnresolvedNS(const std::string &domain) {
    std::vector<std::pair<std::string, std::string>> nameServers = getAllNameServers(domain);
    if (!nameServers.empty()) return nameServers[0].second;
    return "";
}
