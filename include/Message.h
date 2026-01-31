#ifndef MESSAGE_H
#define MESSAGE_H

#include <cstddef>
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
    std::optional<std::string> Get(const std::string& key);
    bool Has(const std::string& key);
    void Set(const std::string& key, const std::string& value);
    std::vector<std::byte> Serialize();
    bool Deserialize(const std::vector<std::byte>& data);
    std::string ToString();
};

#endif