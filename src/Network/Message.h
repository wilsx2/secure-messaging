#pragma once

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

    struct WireFormat
    {
        static constexpr uint16_t MAX_NUM_PAIRS = 16u;
        static constexpr uint32_t MAX_STRING_LEN = UINT32_MAX;
    };

    public:
    Message() = default;
    Message(std::initializer_list<std::pair<std::string, std::string>> list);
    const std::string& Get(const std::string& key) const;
    std::optional<std::string> TryGet(const std::string& key) const;
    template<typename... Args>
    bool HasAll(const Args&... keys) const;
    bool Has(const std::string& key) const;
    void Set(const std::string& key, const std::string& value);
    std::vector<uint8_t> Serialize() const;
    bool Deserialize(const std::vector<uint8_t>& data);
    std::string ToString() const;

    static Message Error(const std::string& content);
};

template<typename... Args>
bool Message::HasAll(const Args&... keys) const
{
    return (Has(keys) && ...);
}