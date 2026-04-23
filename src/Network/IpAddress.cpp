#include "Network/IpAddress.h"
#include <netinet/in.h>

IpAddress::IpAddress(uint32_t value) : _value(value) { }

IpAddress::IpAddress(uint8_t byte1, uint8_t byte2, uint8_t byte3, uint8_t byte4)
    : IpAddress((static_cast<uint32_t>(byte1) << 24)|
                (static_cast<uint32_t>(byte2) << 16)| 
                (static_cast<uint32_t>(byte3) <<  8)| 
                (static_cast<uint32_t>(byte4) <<  0))
{ }

std::string IpAddress::ToString() const
{
    return std::to_string(static_cast<uint8_t>(_value >> 24))
     +"."+ std::to_string(static_cast<uint8_t>(_value >> 16))
     +"."+ std::to_string(static_cast<uint8_t>(_value >> 8))
     +"."+ std::to_string(static_cast<uint8_t>(_value >> 0));
}
uint32_t IpAddress::ToInteger() const
{
    return _value;
}

IpAddress IpAddress::LocalHost()
{
    return INADDR_LOOPBACK;
}