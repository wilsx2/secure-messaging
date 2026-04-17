#include "Network/Serialization.h"

#include <cstring>

ByteReader::ByteReader(const std::vector<uint8_t>& bytes)
    : _bytes(bytes), _start(0)
{ }
bool ByteReader::Peek(void* value, uint64_t size)
{
    bool success = _bytes.size() - _start >= size;

    if (success)
        std::memcpy(value, _bytes.data() + _start, size);

    return success;
}
bool ByteReader::Read(void* value, uint64_t size)
{
    bool success = Peek(value, size);

    if (success)
        _start += size;

    return success;
}

ByteWriter::ByteWriter(uint64_t size)
    : _bytes(size), _start(0)
{ }
bool ByteWriter::Write(void*const value, uint64_t size)
{
    bool success = _bytes.size() - _start >= size;

    if (success)
    {
        std::memcpy(_bytes.data() + _start, value, size);
        _start += size;
    }

    return success;
}
std::vector<uint8_t>&& ByteWriter::Move()
{
    return std::move(_bytes);
}