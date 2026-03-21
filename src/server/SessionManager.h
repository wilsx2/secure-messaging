#ifndef SESSION_MANAGER_H
#define SESSION_MANAGER_H

#include "network/SecureChannel.h"
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
        std::deque<std::chrono::time_point<std::chrono::system_clock>> requests_last_minute;
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

    // Rate Limiting
    int GetRequestsLastMinute(int fd);
    void SubmitRequestTimestamp(int fd);

    friend Session;
};

#endif