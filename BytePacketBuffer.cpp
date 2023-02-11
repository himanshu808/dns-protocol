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

