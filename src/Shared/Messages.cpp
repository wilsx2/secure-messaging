#include "Shared/Messages.h"
#include "Network/Serialization.h"

bool Ping::Serialize(std::vector<uint8_t>& bytes)
{
    uint8_t type_id = TypeId;

    ByteWriter writer(bytes);
    return writer.Write(&type_id, sizeof(type_id));
}
bool Ping::Deserialize(const std::vector<uint8_t>& bytes)
{
    uint8_t type_id;

    ByteReader reader(bytes);
    return reader.Read(&type_id, sizeof(type_id))
        && type_id == TypeId;
}

bool Login::Serialize(std::vector<uint8_t>& bytes)
{
    uint8_t type_id = TypeId;
    uint64_t username_size = username.size();
    uint64_t password_size = password.size();

    ByteWriter writer(bytes);
    return writer.Write(&type_id, sizeof(type_id))
        && writer.Write(&username_size, sizeof(username_size))
        && writer.Write(username.data(), username_size)
        && writer.Write(&password_size, sizeof(password_size))
        && writer.Write(password.data(), password_size);
}
bool Login::Deserialize(const std::vector<uint8_t>& bytes)
{
    uint8_t type_id;
    uint64_t username_size;
    uint64_t password_size;

    ByteReader reader(bytes);
    return reader.Read(&type_id, sizeof(type_id))
        && type_id == TypeId
        && reader.Read(&username_size, sizeof(username_size))
        && (username.resize(username_size), reader.Read(username.data(), username_size))
        && reader.Read(&password_size, sizeof(password_size))
        && (password.resize(password_size), reader.Read(password.data(), password_size));
}

bool Register::Serialize(std::vector<uint8_t>& bytes)
{
    uint8_t type_id = TypeId;
    uint64_t username_size = username.size();
    uint64_t password_size = password.size();

    ByteWriter writer(bytes);
    return writer.Write(&type_id, sizeof(type_id))
        && writer.Write(&username_size, sizeof(username_size))
        && writer.Write(username.data(), username_size)
        && writer.Write(&password_size, sizeof(password_size))
        && writer.Write(password.data(), password_size);
}
bool Register::Deserialize(const std::vector<uint8_t>& bytes)
{
    uint8_t type_id;
    uint64_t username_size;
    uint64_t password_size;

    ByteReader reader(bytes);
    return reader.Read(&type_id, sizeof(type_id))
        && type_id == TypeId
        && reader.Read(&username_size, sizeof(username_size))
        && (username.resize(username_size), reader.Read(username.data(), username_size))
        && reader.Read(&password_size, sizeof(password_size))
        && (password.resize(password_size), reader.Read(password.data(), password_size));
}

bool Logout::Serialize(std::vector<uint8_t>& bytes)
{
    uint8_t type_id = TypeId;

    ByteWriter writer(bytes);
    return writer.Write(&type_id, sizeof(type_id));
}
bool Logout::Deserialize(const std::vector<uint8_t>& bytes)
{
    uint8_t type_id;

    ByteReader reader(bytes);
    return reader.Read(&type_id, sizeof(type_id))
        && type_id == TypeId;
}

bool DeleteAccount::Serialize(std::vector<uint8_t>& bytes)
{
    uint8_t type_id = TypeId;

    ByteWriter writer(bytes);
    return writer.Write(&type_id, sizeof(type_id));
}
bool DeleteAccount::Deserialize(const std::vector<uint8_t>& bytes)
{
    uint8_t type_id;

    ByteReader reader(bytes);
    return reader.Read(&type_id, sizeof(type_id))
        && type_id == TypeId;
}

bool ChangePassword::Serialize(std::vector<uint8_t>& bytes)
{
    uint8_t type_id = TypeId;
    uint64_t new_password_size = new_password.size();

    ByteWriter writer(bytes);
    return writer.Write(&type_id, sizeof(type_id))
        && writer.Write(&new_password_size, sizeof(new_password_size))
        && writer.Write(new_password.data(), new_password_size);
}
bool ChangePassword::Deserialize(const std::vector<uint8_t>& bytes)
{
    uint8_t type_id;
    uint64_t new_password_size;

    ByteReader reader(bytes);
    return reader.Read(&type_id, sizeof(type_id))
        && type_id == TypeId
        && reader.Read(&new_password_size, sizeof(new_password_size))
        && (new_password.resize(new_password_size), reader.Read(new_password.data(), new_password_size));
}

bool ActiveUsers::Serialize(std::vector<uint8_t>& bytes)
{
    uint8_t type_id = TypeId;

    ByteWriter writer(bytes);
    return writer.Write(&type_id, sizeof(type_id));
}
bool ActiveUsers::Deserialize(const std::vector<uint8_t>& bytes)
{
    uint8_t type_id;

    ByteReader reader(bytes);
    return reader.Read(&type_id, sizeof(type_id))
        && type_id == TypeId;
}

bool SendChat::Serialize(std::vector<uint8_t>& bytes)
{
    uint8_t type_id = TypeId;
    uint64_t to_size = to.size();
    uint64_t content_size = content.size();

    ByteWriter writer(bytes);
    return writer.Write(&type_id, sizeof(type_id))
        && writer.Write(&to_size, sizeof(to_size))
        && writer.Write(to.data(), to_size)
        && writer.Write(&content_size, sizeof(content_size))
        && writer.Write(content.data(), content_size);
}
bool SendChat::Deserialize(const std::vector<uint8_t>& bytes)
{
    uint8_t type_id;
    uint64_t to_size;
    uint64_t content_size;

    ByteReader reader(bytes);
    return reader.Read(&type_id, sizeof(type_id))
        && type_id == TypeId
        && reader.Read(&to_size, sizeof(to_size))
        && (to.resize(to_size), reader.Read(to.data(), to_size))
        && reader.Read(&content_size, sizeof(content_size))
        && (content.resize(content_size), reader.Read(content.data(), content_size));
}

bool ReceiveChat::Serialize(std::vector<uint8_t>& bytes)
{
    uint8_t type_id = TypeId;
    uint64_t from_size = from.size();
    uint64_t content_size = content.size();

    ByteWriter writer(bytes);
    return writer.Write(&type_id, sizeof(type_id))
        && writer.Write(&from_size, sizeof(from_size))
        && writer.Write(from.data(), from_size)
        && writer.Write(&content_size, sizeof(content_size))
        && writer.Write(content.data(), content_size);
}
bool ReceiveChat::Deserialize(const std::vector<uint8_t>& bytes)
{
    uint8_t type_id;
    uint64_t from_size;
    uint64_t content_size;

    ByteReader reader(bytes);
    return reader.Read(&type_id, sizeof(type_id))
        && type_id == TypeId
        && reader.Read(&from_size, sizeof(from_size))
        && (from.resize(from_size), reader.Read(from.data(), from_size))
        && reader.Read(&content_size, sizeof(content_size))
        && (content.resize(content_size), reader.Read(content.data(), content_size));
}

bool Success::Serialize(std::vector<uint8_t>& bytes)
{
    uint8_t type_id = TypeId;

    ByteWriter writer(bytes);
    return writer.Write(&type_id, sizeof(type_id));
}
bool Success::Deserialize(const std::vector<uint8_t>& bytes)
{
    uint8_t type_id;

    ByteReader reader(bytes);
    return reader.Read(&type_id, sizeof(type_id))
        && type_id == TypeId;
}

bool Failure::Serialize(std::vector<uint8_t>& bytes)
{
    uint8_t type_id = TypeId;
    uint64_t what_size = what.size();

    ByteWriter writer(bytes);
    return writer.Write(&type_id, sizeof(type_id))
        && writer.Write(&what_size, sizeof(what_size))
        && writer.Write(what.data(), what_size);
}
bool Failure::Deserialize(const std::vector<uint8_t>& bytes)
{
    uint8_t type_id;
    uint64_t what_size;

    ByteReader reader(bytes);
    return reader.Read(&type_id, sizeof(type_id))
        && type_id == TypeId
        && reader.Read(&what_size, sizeof(what_size))
        && (what.resize(what_size), reader.Read(what.data(), what_size));
}

bool StringList::Serialize(std::vector<uint8_t>& bytes)
{
    uint8_t type_id = TypeId;
    uint64_t list_size = value.size();

    ByteWriter writer(bytes);
    if (!(writer.Write(&type_id, sizeof(type_id)))
    ||  !(writer.Write(&list_size, sizeof(list_size))))
        return false;

    for (auto& string : value)
    {
        uint64_t string_size = string.length();
        if ((!writer.Write(&string_size, sizeof(string_size))
        ||  (!writer.Write(string.data(), string_size))))
            return false;
    }
    return true;
}
bool StringList::Deserialize(const std::vector<uint8_t>& bytes)
{
    uint8_t type_id;
    uint64_t list_size;
    uint64_t string_size;

    ByteReader reader(bytes);
    if (!reader.Read(&type_id, sizeof(type_id))
    ||  type_id != TypeId
    ||  !reader.Read(&list_size, sizeof(list_size)))
        return false;
    
    value.resize(list_size);
    for (auto& string : value)
    {
        if (!reader.Read(&string_size, sizeof(string_size))
        ||  (string.resize(string_size), !reader.Read(string.data(), string_size)))
            return false;
    }

    return true;
}