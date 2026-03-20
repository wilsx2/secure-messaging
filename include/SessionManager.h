#ifndef SESSION_MANAGER_H
#define SESSION_MANAGER_H

#include "SecureChannel.h"
#include <string>
#include <map>

struct Session
{
    SecureChannel channel;
    std::string username;
    bool authenticated;
};

class SessionManager
{
    private:
    std::map<int, Session> _sessions;
    std::map<std::string, int> _user_to_fd;
    
    public:
    SessionManager() = default;
    bool CreateSession(int fd);
    void DestroySession(int fd);

    bool Has(int fd);
    bool Has(std::string username);
    Session& Get(int fd);
    Session& Get(std::string username);
    bool Authenticate(int fd, std::string username);
    bool Unauthenticate(int fd);
};

#endif