#pragma once

#include <string>
#include <vector>
#include <cstdint>

struct Message
{
    virtual bool Serialize(std::vector<uint8_t>& bytes) = 0;
    virtual bool Deserialize(const std::vector<uint8_t>& bytes) = 0;
    virtual std::string ToString() = 0;
};