#pragma once

#include "Network/SecureChannel.h"
#include <string>
#include <map>
#include <deque>
#include <chrono>

class SessionManager
{
    private:
    struct Session
    {
        SecureChannel channel;
        std::string username;
        bool authenticated;
    };
    std::map<int, Session> _sessions;
    std::map<std::string, int> _user_to_fd;
    
    public:
    SessionManager() = default;

    bool Create(int fd);
    bool Destroy(int fd);
    
    bool IsEstablished(int fd) const;
    bool IsEstablished(const std::string& username) const;

    // Communication
    SecureChannel& GetChannel(int fd);
    SecureChannel& GetChannel(const std::string& username);

    // Auth
    bool IsAuthenticated(int fd) const;
    const std::string& GetUsername(int fd) const;
    bool Authenticate(int fd, const std::string& username);
    bool Unauthenticate(int fd);
};