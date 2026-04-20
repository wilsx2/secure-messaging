#pragma once

#include "Network/Message.h"
#include <string>
#include <variant>

struct Login : public Message
{
    static constexpr uint8_t TypeId {0};
    std::string username;
    std::string password;

    Login() = default;
    Login(std::string username, std::string password)
        : username(username), password(password) { }
    bool Serialize(std::vector<uint8_t>& bytes);
    bool Deserialize(const std::vector<uint8_t>& bytes);
};

struct Register : public Message
{
    static constexpr uint8_t TypeId {1};

    std::string username;
    std::string password;

    Register() = default;
    Register(std::string username, std::string password)
        : username(username), password(password) { }
    bool Serialize(std::vector<uint8_t>& bytes);
    bool Deserialize(const std::vector<uint8_t>& bytes);
};

struct Logout : public Message
{
    static constexpr uint8_t TypeId {2};

    Logout() = default;
    bool Serialize(std::vector<uint8_t>& bytes);
    bool Deserialize(const std::vector<uint8_t>& bytes);
};

struct DeleteAccount : public Message
{
    static constexpr uint8_t TypeId {3};

    DeleteAccount() = default;
    bool Serialize(std::vector<uint8_t>& bytes);
    bool Deserialize(const std::vector<uint8_t>& bytes);
};

struct ChangePassword : public Message
{
    static constexpr uint8_t TypeId {4};

    std::string new_password;

    ChangePassword() = default;
    ChangePassword(std::string new_password)
        : new_password(new_password) { }
    bool Serialize(std::vector<uint8_t>& bytes);
    bool Deserialize(const std::vector<uint8_t>& bytes);
};

struct ActiveUsers : public Message
{
    static constexpr uint8_t TypeId {5};

    ActiveUsers() = default;
    bool Serialize(std::vector<uint8_t>& bytes);
    bool Deserialize(const std::vector<uint8_t>& bytes);
};

struct SendChat : public Message
{
    static constexpr uint8_t TypeId {6};

    std::string to;
    std::string content;

    SendChat() = default;
    SendChat(std::string to, std::string content)
        : to(to), content(content) { }
    bool Serialize(std::vector<uint8_t>& bytes);
    bool Deserialize(const std::vector<uint8_t>& bytes);
};

struct ReceiveChat : public Message
{
    static constexpr uint8_t TypeId {7};

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
    static constexpr uint8_t TypeId {8};

    Success() = default;
    bool Serialize(std::vector<uint8_t>& bytes);
    bool Deserialize(const std::vector<uint8_t>& bytes);
};

struct Failure : public Message
{
    static constexpr uint8_t TypeId {9};

    std::string what;

    Failure() = default;
    Failure(std::string what)
        : what(what) { }
    bool Serialize(std::vector<uint8_t>& bytes);
    bool Deserialize(const std::vector<uint8_t>& bytes);
};

struct StringList : public Message
{
    static constexpr uint8_t TypeId {10};

    std::vector<std::string> value;

    StringList() = default;
    StringList(std::vector<std::string>&& value)
        : request_id(request_id), value(value) { }
    bool Serialize(std::vector<uint8_t>& bytes);
    bool Deserialize(const std::vector<uint8_t>& bytes);
};

enum class RequestError { Send, Disconnected, Timeout, Serialization, Deserialization };
using Request = std::variant<Login, Register, SendChat>;
using Response = std::variant<Success, Failure, ReceiveChat>;