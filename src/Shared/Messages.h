#pragma once

#include "Network/Message.h"
#include <string>
#include <variant>

struct Ping : public Message
{
    static constexpr uint8_t TypeId {1};

    Ping() = default;
    bool Serialize(std::vector<uint8_t>& bytes);
    bool Deserialize(const std::vector<uint8_t>& bytes);
    std::string ToString();
};

struct Login : public Message
{
    static constexpr uint8_t TypeId {2};
    std::string username;
    std::string password;

    Login() = default;
    Login(std::string username, std::string password)
        : username(username), password(password) { }
    bool Serialize(std::vector<uint8_t>& bytes);
    bool Deserialize(const std::vector<uint8_t>& bytes);
    std::string ToString();
};

struct Register : public Message
{
    static constexpr uint8_t TypeId {3};

    std::string username;
    std::string password;

    Register() = default;
    Register(std::string username, std::string password)
        : username(username), password(password) { }
    bool Serialize(std::vector<uint8_t>& bytes);
    bool Deserialize(const std::vector<uint8_t>& bytes);
    std::string ToString();
};

struct Logout : public Message
{
    static constexpr uint8_t TypeId {4};

    Logout() = default;
    bool Serialize(std::vector<uint8_t>& bytes);
    bool Deserialize(const std::vector<uint8_t>& bytes);
    std::string ToString();
};

struct DeleteAccount : public Message
{
    static constexpr uint8_t TypeId {5};

    DeleteAccount() = default;
    bool Serialize(std::vector<uint8_t>& bytes);
    bool Deserialize(const std::vector<uint8_t>& bytes);
    std::string ToString();
};

struct ChangePassword : public Message
{
    static constexpr uint8_t TypeId {6};

    std::string new_password;

    ChangePassword() = default;
    ChangePassword(std::string new_password)
        : new_password(new_password) { }
    bool Serialize(std::vector<uint8_t>& bytes);
    bool Deserialize(const std::vector<uint8_t>& bytes);
    std::string ToString();
};

struct ActiveUsers : public Message
{
    static constexpr uint8_t TypeId {7};

    ActiveUsers() = default;
    bool Serialize(std::vector<uint8_t>& bytes);
    bool Deserialize(const std::vector<uint8_t>& bytes);
    std::string ToString();
};

struct SendChat : public Message
{
    static constexpr uint8_t TypeId {8};

    std::string to;
    std::string content;

    SendChat() = default;
    SendChat(std::string to, std::string content)
        : to(to), content(content) { }
    bool Serialize(std::vector<uint8_t>& bytes);
    bool Deserialize(const std::vector<uint8_t>& bytes);
    std::string ToString();
};

struct ReceiveChat : public Message
{
    static constexpr uint8_t TypeId {9};

    std::string from;
    std::string content;

    ReceiveChat() = default;
    ReceiveChat(std::string from, std::string content)
        : from(from), content(content) { }
    bool Serialize(std::vector<uint8_t>& bytes);
    bool Deserialize(const std::vector<uint8_t>& bytes);
    std::string ToString();
};

struct Success : public Message
{
    static constexpr uint8_t TypeId {10};

    Success() = default;
    bool Serialize(std::vector<uint8_t>& bytes);
    bool Deserialize(const std::vector<uint8_t>& bytes);
    std::string ToString();
};

struct Failure : public Message
{
    static constexpr uint8_t TypeId {11};

    std::string what;

    Failure() = default;
    Failure(std::string what)
        : what(what) { }
    bool Serialize(std::vector<uint8_t>& bytes);
    bool Deserialize(const std::vector<uint8_t>& bytes);
    std::string ToString();
};

struct StringList : public Message
{
    static constexpr uint8_t TypeId {12};

    std::vector<std::string> value;

    StringList() = default;
    StringList(std::vector<std::string>&& value)
        : value(value) { }
    bool Serialize(std::vector<uint8_t>& bytes);
    bool Deserialize(const std::vector<uint8_t>& bytes);
    std::string ToString();
};

enum class RequestError { Send, Disconnected, Timeout, Serialization, Deserialization };
using Request = std::variant<Ping, Login, Register, SendChat>;
using Response = std::variant<Success, Failure, ReceiveChat>;