#ifndef MESSAGE_H
#define MESSAGE_H

#include <cstdint>
#include <vector>
#include <optional>
#include <string>
#include <map>

class Message
{
    private:
    std::map<std::string,std::string> _key_values;

    public:
    Message() = default;
    std::optional<std::string> Get(const std::string& key) const;
    bool Has(const std::string& key) const;
    void Set(const std::string& key, const std::string& value);
    std::vector<uint8_t> Serialize();
    bool Deserialize(const std::vector<uint8_t>& data);
    std::string ToString();
};

#endif