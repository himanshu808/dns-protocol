#include "BytePacketBuffer.hpp"

[[ noreturn ]] void BytePacketBuffer::raiseError() {
    throw "Error!!!";
}

void BytePacketBuffer::validatePos(unsigned int position) {
    if (position >= 512) raiseError();
}

unsigned BytePacketBuffer::getPos() { return pos; }

void BytePacketBuffer::step(unsigned int steps) { pos += steps; }

void BytePacketBuffer::seek(unsigned int position) { pos = position; }

uint8_t BytePacketBuffer::read() {
    validatePos(pos);
    return buf[pos++];
}

uint8_t BytePacketBuffer::getSingleByte(unsigned int position) {
    validatePos(position);
    return buf[position];
}

std::vector<uint8_t> BytePacketBuffer::getBytes(unsigned int start, unsigned int n) {
    validatePos(start + n);
    std::vector<uint8_t> retVal;
    for (unsigned i = start; i < start + n; i++) retVal.push_back(buf[i]);
    return retVal;
}

uint16_t BytePacketBuffer::read2Bytes() { return (read() << 8 | read()); }

uint32_t BytePacketBuffer::read4Bytes() {
    return (read() << 24
            | read() << 16
            | read() << 8
            | read());
}

void BytePacketBuffer::readDomainName(std::string &domain) {
    unsigned curr = pos;
    bool jumped = false;
    unsigned len;
    uint8_t byte2;

    unsigned maxJumps = 5;
    unsigned jumpsSoFar = 0;

    std::string delimiter;
    std::vector<uint8_t> byteRangeBuf;

    do {
        if (jumpsSoFar > maxJumps) throw "Max jumps exceed while parsing domain name";

        len = getSingleByte(curr);

        if ((len & 0xC0) == 0xC0) {
            if (!jumped) seek(curr + 2);

            byte2 = getSingleByte(curr + 1);
            curr = ((len ^ 0xC0) << 8) | byte2;
            jumped = true;
            jumpsSoFar++;
        } else {
            curr++;
            if (!len) break;

            domain += delimiter;
            byteRangeBuf = getBytes(curr, len);
            domain += std::string(byteRangeBuf.begin(), byteRangeBuf.end());

            delimiter = ".";
            curr += len;
        }
    } while (len);

    if (!jumped) seek(curr);
}

void BytePacketBuffer::write1Byte(uint8_t val) {
    validatePos(pos);
    buf[pos++] = val;
}

void BytePacketBuffer::write2Bytes(uint16_t val) {
    write1Byte(static_cast<uint8_t>(val >> 8));
    write1Byte(static_cast<uint8_t>(val & 0xFF));
}

void BytePacketBuffer::write4Bytes(uint32_t val) {
    // TODO: try reusing write2Bytes()?
    // TODO: do we need to & each time?
    write1Byte(static_cast<uint8_t>(val >> 24) & 0xFF);
    write1Byte(static_cast<uint8_t>(val >> 16) & 0xFF);
    write1Byte(static_cast<uint8_t>(val >> 8) & 0xFF);
    write1Byte(static_cast<uint8_t>(val & 0xFF));
}

void BytePacketBuffer::writeDomainName(const std::string &domain) {
    size_t prev = 0;
    size_t next = 0;
    char delimiter = '.';
    std::string label;
    unsigned len;

    while((next = domain.find(delimiter, prev)) != std::string::npos) {
        label = domain.substr(prev, next-prev);
        len = label.length();

        if (len > 63) throw "Label exceeds 63 characters";
        write1Byte(len);

        for(const char& c: label) write1Byte(static_cast<uint8_t>(c));

        prev = next + 1;
    }
    write1Byte(0);  // terminate using a 0 indicating the end of label

}

