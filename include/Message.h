#ifndef MESSAGE_H
#define MESSAGE_H

#include <cstdint>
#include <initializer_list>
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
    Message(std::initializer_list<std::pair<std::string, std::string>> list);
    std::optional<std::string> Get(const std::string& key) const;
    template<typename... Args>
    bool HasAll(const Args&... keys) const;
    bool Has(const std::string& key) const;
    void Set(const std::string& key, const std::string& value);
    std::vector<uint8_t> Serialize() const;
    bool Deserialize(const std::vector<uint8_t>& data);
    std::string ToString() const;
};

template<typename... Args>
bool Message::HasAll(const Args&... keys) const
{
    return (Has(keys) && ...);
}

#endif