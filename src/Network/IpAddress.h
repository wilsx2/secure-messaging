#pragma once

#include <string>
#include <cstdint>

class IpAddress
{
    private:
    uint32_t _value;

    public:
    IpAddress(uint32_t value);
    IpAddress(uint8_t byte1, uint8_t byte2, uint8_t byte3, uint8_t byte4);
    std::string ToString() const;
    uint32_t ToInteger() const;

    static IpAddress LocalHost();
};