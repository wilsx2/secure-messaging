#pragma once

#include <vector>
#include <string>
#include <cstdint>

struct Message
{
    virtual std::vector<uint8_t> Serialize(std::vector<uint8_t>& data);
    virtual bool Deserialize(const std::vector<uint8_t>& data);
};