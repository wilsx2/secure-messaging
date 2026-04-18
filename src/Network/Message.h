#pragma once

#include <vector>
#include <string>
#include <cstdint>
#include <typeinfo>
#include <arpa/inet.h> 
#include <cstring>

struct Message
{
    // Interface
    virtual bool Serialize(std::vector<uint8_t>& bytes) = 0;
    virtual bool Deserialize(const std::vector<uint8_t>& bytes) = 0;

    // Static helper functions
    template <typename T>
    static uint64_t GetTypeHash();
};

template <typename T>
uint64_t Message::GetTypeHash()
{
    return static_cast<uint64_t>(typeid(T).hash_code());
}