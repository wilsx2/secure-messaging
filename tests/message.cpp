#include "Network/Message.h"
#include "Network/Serialization.h"
#include <iostream>

struct Request : public Message
{
    uint64_t id;
    std::string content;

    Request() = default;
    Request(uint64_t id, std::string content)
        : id(id), content(content) { }
    bool Serialize(std::vector<uint8_t>& bytes)
    {
        uint64_t type_hash = Message::GetTypeHash<Request>();
        uint64_t content_size = content.size();

        ByteWriter writer(bytes);
        if (!writer.Write(&type_hash, sizeof(type_hash)))
            return false;
        if (!writer.Write(&id, sizeof(id)))
            return false;
        if (!writer.Write(&content_size, sizeof(content_size)))
            return false;
        if (!writer.Write(content.data(), content_size))
            return false;
        return true;
    }
    bool Deserialize(const std::vector<uint8_t>& bytes)
    {
        uint64_t type_hash;
        uint64_t content_size;

        ByteReader reader(bytes);
        if (!reader.Read(&type_hash, sizeof(type_hash))
           || type_hash != Message::GetTypeHash<Request>())
            return false;
        if (!reader.Read(&id, sizeof(id)))
            return false;
        if (!reader.Read(&content_size, sizeof(content_size)))
            return false;
        content.resize(content_size);
        if (!reader.Read(content.data(), content_size))
            return false;
        return true;
    }
};

int main()
{
    std::vector<uint8_t> bytes (1024);

    Request a (156, "Hello Wurld");    
    Request b;
    if (!a.Serialize(bytes) || !b.Deserialize(bytes))
        return 1;

    std::cout << "a id:" << std::to_string(a.id) << " content:" << a.content << std::endl;
    std::cout << "b id:" << std::to_string(b.id) << " content:" << b.content << std::endl;
    return !(a.id == b.id && a.content == b.content);
}