#ifndef BYTEPACKETBUFFER_H
#define BYTEPACKETBUFFER_H


#include <cstdint>
#include <vector>
#include <string>


class BytePacketBuffer {
private:
    unsigned pos;

public:
//    std::vector<uint8_t> buf;
    uint8_t buf[512];  // TODO: do we always require a buffer of size 512?
    BytePacketBuffer() : pos(0), buf() {}

    unsigned getPos() const;
    void step(unsigned steps);
    void seek(unsigned position);
    uint8_t read();
    uint8_t getSingleByte(unsigned position);
    std::vector<uint8_t> getBytes(unsigned start, unsigned n);
    uint16_t read2Bytes();
    uint32_t read4Bytes();
    void readDomainName(std::string &domain);
    void validatePos(unsigned position);
    [[ noreturn ]] void raiseError();
    void write1Byte(uint8_t val);
    void write2Bytes(uint16_t val);
    void write4Bytes(uint32_t val);
    void writeDomainName(const std::string &domain);
    void set1Byte(unsigned position, uint8_t val);
    void set2Bytes(unsigned position, uint16_t val);
};

#endif