#include "Message.h"
#include <cstring>
#include <iostream>

std::optional<std::string> Message::Get(const std::string& key)
{
    auto it = _key_values.find(key);
    if (it == _key_values.end())
        return std::nullopt;
    return it->second;
}
bool Message::Has(const std::string& key)
{
    return _key_values.count(key);
}
void Message::Set(const std::string& key, const std::string& value)
{
    _key_values.insert_or_assign(key, value);
}

std::vector<std::byte> Message::Serialize()
{
    std::vector<std::byte> data;

    return data;
}

bool Message::Deserialize(const std::vector<std::byte>& data)
{
    return false;
}
std::string Message::ToString()
{
    std::string output;
    for (auto& pair : _key_values)
    {
        if (output.size() > 0)
            output += " | ";
        output += pair.first + ":" + pair.second;
    }
    return output;
}