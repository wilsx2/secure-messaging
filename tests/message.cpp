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
    std::vector<uint8_t> Serialize()
    {
        uint64_t type_hash = Message::GetTypeHash<Request>();
        uint64_t content_size = content.size();

        ByteWriter writer(sizeof(type_hash) + sizeof(id) + sizeof(content_size) + content_size);
        writer.Write(&type_hash, sizeof(type_hash));
        std::cout << "Wrote type hash\n";
        writer.Write(&id, sizeof(id));
        std::cout << "Wrote ID\n";
        writer.Write(&content_size, sizeof(content_size));
        std::cout << "Wrote content size\n";
        writer.Write(content.data(), content_size);
        std::cout << "Wrote type hash\n";
        return writer.Move();
    }
    bool Deserialize(const std::vector<uint8_t>& bytes)
    {
        uint64_t type_hash;
        uint64_t content_size;

        ByteReader reader(bytes);
        if(!reader.Read(&type_hash, sizeof(type_hash))
           || type_hash != Message::GetTypeHash<Request>())
            return false;
        std::cout << "Read type\n";
        if(!reader.Read(&id, sizeof(id)))
            return false;
        std::cout << "Read ID\n";
        if(!reader.Read(&content_size, sizeof(content_size)))
            return false;
        std::cout << "Read content size\n";
        content.resize(content_size);
        if(!reader.Read(content.data(), content_size))
            return false;
        std::cout << "Read content\n";
        return true;
    }
};

int main()
{
    std::vector<uint8_t> bytes;

    Request a (156, "Hello Wurld");    
    Request b;
    if (b.Deserialize(a.Serialize()) == false)
        return 1;

    std::cout << "a id:" << std::to_string(a.id) << " content:" << a.content << std::endl;
    std::cout << "b id:" << std::to_string(b.id) << " content:" << b.content << std::endl;
    return !(a.id == b.id && a.content == b.content);
}