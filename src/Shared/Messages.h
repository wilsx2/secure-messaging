#pragma once

#include "Network/Message.h"

struct Login : public Message
{
    uint64_t id;
    std::string username;
    std::string password;

    Login(uint64_t id, std::string username, std::string password)
        : id(id), username(username), password(password) { }
};

struct Register : public Message
{
    uint64_t id;
    std::string username;
    std::string password;

    Register(uint64_t id, std::string username, std::string password)
        : id(id), username(username), password(password) { }
};

struct DeleteAccount : public Message
{
    uint64_t id;

    DeleteAccount(uint64_t id): id(id) { }
};

struct ChangePassword : public Message
{
    uint64_t id;
    std::string new_password;

    ChangePassword(uint64_t id, std::string new_password)
        : id(id), new_password(new_password) { }
};

struct SendChat : public Message
{
    uint64_t id;
    std::string from;
    std::string to;
    std::string content;

    SendChat(uint64_t id, std::string from, std::string to, std::string content)
        : id(id), from(from), to(to), content(content) { }
};

struct Status : public Message
{
    uint64_t request_id;
    bool successful;

    Status(uint64_t request_id, bool successful)
        : request_id(request_id), successful(successful) { }
};

struct StringList : public Message
{
    uint64_t request_id;
    std::vector<std::string> value;

    StringList(uint64_t request_id, std::vector<std::string>&& value)
        : request_id(request_id), value(value) { }
};