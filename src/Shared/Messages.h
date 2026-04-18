#pragma once

#include "Network/Message.h"

struct Login : public Message
{
    uint64_t id;
    std::string username;
    std::string password;

    Login() = default;
    Login(uint64_t id, std::string username, std::string password)
        : id(id), username(username), password(password) { }
    bool Serialize(std::vector<uint8_t>& bytes);
    bool Deserialize(const std::vector<uint8_t>& bytes);
};

struct Register : public Message
{
    uint64_t id;
    std::string username;
    std::string password;

    Register() = default;
    Register(uint64_t id, std::string username, std::string password)
        : id(id), username(username), password(password) { }
    bool Serialize(std::vector<uint8_t>& bytes);
    bool Deserialize(const std::vector<uint8_t>& bytes);
};

struct DeleteAccount : public Message
{
    uint64_t id;

    DeleteAccount() = default;
    DeleteAccount(uint64_t id): id(id) { }
    bool Serialize(std::vector<uint8_t>& bytes);
    bool Deserialize(const std::vector<uint8_t>& bytes);
};

struct ChangePassword : public Message
{
    uint64_t id;
    std::string new_password;

    ChangePassword() = default;
    ChangePassword(uint64_t id, std::string new_password)
        : id(id), new_password(new_password) { }
    bool Serialize(std::vector<uint8_t>& bytes);
    bool Deserialize(const std::vector<uint8_t>& bytes);
};

struct ActiveUsers : public Message
{
    uint64_t id;

    ActiveUsers() = default;
    ActiveUsers(uint64_t id): id(id) { }
    bool Serialize(std::vector<uint8_t>& bytes);
    bool Deserialize(const std::vector<uint8_t>& bytes);
};

struct SendChat : public Message
{
    uint64_t id;
    std::string from;
    std::string to;
    std::string content;

    SendChat() = default;
    SendChat(uint64_t id, std::string from, std::string to, std::string content)
        : id(id), from(from), to(to), content(content) { }
    bool Serialize(std::vector<uint8_t>& bytes);
    bool Deserialize(const std::vector<uint8_t>& bytes);
};

struct Status : public Message
{
    uint64_t request_id;
    bool successful;

    Status() = default;
    Status(uint64_t request_id, bool successful)
        : request_id(request_id), successful(successful) { }
    bool Serialize(std::vector<uint8_t>& bytes);
    bool Deserialize(const std::vector<uint8_t>& bytes);
};

struct StringList : public Message
{
    uint64_t request_id;
    std::vector<std::string> value;

    StringList() = default;
    StringList(uint64_t request_id, std::vector<std::string>&& value)
        : request_id(request_id), value(value) { }
    bool Serialize(std::vector<uint8_t>& bytes);
    bool Deserialize(const std::vector<uint8_t>& bytes);
};