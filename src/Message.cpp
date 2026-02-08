#include "Message.h"
#include <arpa/inet.h> 
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

std::vector<uint8_t> Message::Serialize() const
{
    std::vector<uint8_t> bytes;
    
    // Store number of pairs
    uint16_t num_pairs = static_cast<uint16_t>(_key_values.size());
    uint16_t num_pairs_be = htons(num_pairs);
    bytes.insert(
        bytes.end(), 
        reinterpret_cast<uint8_t*>(&num_pairs_be), 
        reinterpret_cast<uint8_t*>(&num_pairs_be) + sizeof(num_pairs_be)
    );
    
    // Store each pair
    for (const auto& _key_value_pair : _key_values)
    {
        uint16_t key_len = static_cast<uint16_t>(_key_value_pair.first.size());
        uint16_t val_len = static_cast<uint16_t>(_key_value_pair.second.size());
        uint16_t key_len_be = htons(key_len);
        uint16_t val_len_be = htons(val_len);

        // Store lengths
        bytes.insert(
            bytes.end(),
            reinterpret_cast<uint8_t*>(&key_len_be),
            reinterpret_cast<uint8_t*>(&key_len_be) + sizeof(key_len_be)
        );
        bytes.insert(
            bytes.end(), 
            reinterpret_cast<uint8_t*>(&val_len_be),
            reinterpret_cast<uint8_t*>(&val_len_be) + sizeof(val_len_be)
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
    uint32_t curr = 0;
    bool success;
    std::map<std::string, std::string> key_values;

    // Read number of pairs
    if (bytes.size() < sizeof(uint16_t))
        return false;
    uint16_t num_pairs_be, num_pairs;
    std::memcpy(&num_pairs_be, &bytes[curr], sizeof(num_pairs_be));
    num_pairs = ntohs(num_pairs_be);
    curr += sizeof(num_pairs_be);

    // Check pair size
    if (num_pairs > WireFormat::MAX_NUM_PAIRS)
        return false;

    // Read each pair
    for (uint16_t i = 0; i < num_pairs; ++i) {
        // Read string sizes
        uint16_t key_len, val_len, key_len_be, val_len_be;
        if (curr + sizeof(key_len) + sizeof(val_len) > bytes.size())
            return false;
        
        std::memcpy(&key_len_be, &bytes[curr], sizeof(key_len_be));
        key_len = ntohs(key_len_be);
        curr += sizeof(key_len);
        std::memcpy(&val_len_be, &bytes[curr], sizeof(val_len_be));
        val_len = ntohs(val_len_be);
        curr += sizeof(val_len);
        
        // Read string contents
        if (curr + key_len + val_len > bytes.size()
            || key_len > WireFormat::MAX_STRING_LEN
            || val_len > WireFormat::MAX_STRING_LEN)
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
std::string Message::ToString() const
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