#include "Message.h"
#include <cstring>
#include <iostream>

Message::Message(std::initializer_list<std::pair<std::string, std::string>> list)
{
    for (auto& pair : list)
    {
        _key_values.emplace(pair);
    }
}

std::optional<std::string> Message::Get(const std::string& key) const
{
    auto it = _key_values.find(key);
    if (it == _key_values.end())
        return std::nullopt;
    return it->second;
}
bool Message::Has(const std::string& key) const
{
    return _key_values.count(key);
}
void Message::Set(const std::string& key, const std::string& value)
{
    _key_values.insert_or_assign(key, value);
}

std::vector<uint8_t> Message::Serialize()
{
    std::vector<uint8_t> bytes;
    
    // Store number of pairs
    std::size_t num_pairs = _key_values.size();
    bytes.insert(
        bytes.end(), 
        reinterpret_cast<uint8_t*>(&num_pairs), 
        reinterpret_cast<uint8_t*>(&num_pairs) + sizeof(num_pairs)
    );
    
    // Store each pair
    for (const auto& _key_value_pair : _key_values)
    {
        std::size_t key_len = _key_value_pair.first.size();
        std::size_t val_len = _key_value_pair.second.size();

        // Store lengths
        bytes.insert(
            bytes.end(), 
            reinterpret_cast<uint8_t*>(&key_len),
            reinterpret_cast<uint8_t*>(&key_len) + sizeof(key_len)
        );
        bytes.insert(
            bytes.end(), 
            reinterpret_cast<uint8_t*>(&val_len),
            reinterpret_cast<uint8_t*>(&val_len) + sizeof(val_len)
        );

        // Store content
        bytes.insert(
            bytes.end(), 
            _key_value_pair.first.begin(),
            _key_value_pair.first.end()
        );
        bytes.insert(
            bytes.end(), 
            _key_value_pair.second.begin(),
            _key_value_pair.second.end()
        );
    }

    return bytes;
}

bool Message::Deserialize(const std::vector<uint8_t>& bytes)
{
    std::size_t curr = 0;
    bool success;
    std::map<std::string, std::string> key_values;

    // Read number of pairs
    if (bytes.size() < sizeof(std::size_t))
        return false;
    std::size_t num_pairs;
    std::memcpy(&num_pairs, &bytes[curr], sizeof(num_pairs));
    curr += sizeof(num_pairs);

    // Read each pair
    for (std::size_t i = 0; i < num_pairs; ++i) {
        // Read string sizes
        std::size_t key_len, val_len;
        if (curr + sizeof(key_len) + sizeof(val_len) > bytes.size())
            return false;
        
        std::memcpy(&key_len, &bytes[curr], sizeof(key_len));
        curr += sizeof(key_len);
        std::memcpy(&val_len, &bytes[curr], sizeof(val_len));
        curr += sizeof(val_len);
        
        // Read string contents
        if (curr + key_len + val_len > bytes.size())
            return false;

        std::string key(bytes.begin() + curr, bytes.begin() + curr + key_len);
        curr += key_len;
        std::string val(bytes.begin() + curr, bytes.begin() + curr + val_len);
        curr += val_len;

        // Insert into pairs
        key_values.emplace(key, val);
    }

    // Update key value pairs
    _key_values = std::move(key_values);
    return true;
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