#pragma once

#include <cstdint>
#include <vector>

class ByteReader
{
    private:
    const std::vector<uint8_t>& _bytes;
    uint64_t _start;

    public:
    ByteReader(const std::vector<uint8_t>& bytes);
    bool Peek(void* value, uint64_t size);
    bool Read(void* value, uint64_t size);
};

class ByteWriter
{
    private:
    std::vector<uint8_t> _bytes;
    uint64_t _start;

    public:
    ByteWriter(uint64_t size);
    bool Write(void*const value, uint64_t size);
    std::vector<uint8_t>&& Move();
};