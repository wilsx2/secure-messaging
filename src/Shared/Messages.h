#pragma once

#include "Network/Message.h"
#include <string>
#include <variant>

struct Login : public Message
{
    static constexpr uint8_t TypeId {0};
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
    static constexpr uint8_t TypeId {1};

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
    static constexpr uint8_t TypeId {2};

    uint64_t id;

    DeleteAccount() = default;
    DeleteAccount(uint64_t id): id(id) { }
    bool Serialize(std::vector<uint8_t>& bytes);
    bool Deserialize(const std::vector<uint8_t>& bytes);
};

struct ChangePassword : public Message
{
    static constexpr uint8_t TypeId {3};

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
    static constexpr uint8_t TypeId {4};

    uint64_t id;

    ActiveUsers() = default;
    ActiveUsers(uint64_t id): id(id) { }
    bool Serialize(std::vector<uint8_t>& bytes);
    bool Deserialize(const std::vector<uint8_t>& bytes);
};

struct SendChat : public Message
{
    static constexpr uint8_t TypeId {5};

    uint64_t id;
    std::string to;
    std::string content;

    SendChat() = default;
    SendChat(uint64_t id, std::string to, std::string content)
        : id(id), to(to), content(content) { }
    bool Serialize(std::vector<uint8_t>& bytes);
    bool Deserialize(const std::vector<uint8_t>& bytes);
};

struct ReceiveChat : public Message
{
    static constexpr uint8_t TypeId {6};

    std::string from;
    std::string content;

    ReceiveChat() = default;
    ReceiveChat(std::string from, std::string content)
        : from(from), content(content) { }
    bool Serialize(std::vector<uint8_t>& bytes);
    bool Deserialize(const std::vector<uint8_t>& bytes);
};

struct Success : public Message
{
    static constexpr uint8_t TypeId {7};

    uint64_t request_id;

    Success() = default;
    Success(uint64_t request_id)
        : request_id(request_id) { }
    bool Serialize(std::vector<uint8_t>& bytes);
    bool Deserialize(const std::vector<uint8_t>& bytes);
};

struct Failure : public Message
{
    static constexpr uint8_t TypeId {8};

    uint64_t request_id;
    std::string what;

    Failure() = default;
    Failure(uint64_t request_id, std::string what)
        : request_id(request_id), what(what) { }
    bool Serialize(std::vector<uint8_t>& bytes);
    bool Deserialize(const std::vector<uint8_t>& bytes);
};

struct StringList : public Message
{
    static constexpr uint8_t TypeId {9};

    uint64_t request_id;
    std::vector<std::string> value;

    StringList() = default;
    StringList(uint64_t request_id, std::vector<std::string>&& value)
        : request_id(request_id), value(value) { }
    bool Serialize(std::vector<uint8_t>& bytes);
    bool Deserialize(const std::vector<uint8_t>& bytes);
};

enum class RequestStatus { Sent, Disconnected, Timeout, SerializationFailed, SendFailed };
using Request = std::variant<Login, Register, SendChat>;
using Response = std::variant<Success, Failure, ReceiveChat>;