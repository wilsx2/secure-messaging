#include "SessionManager.h"

bool SessionManager::CreateSession(int fd)
{
    Session session { SecureChannel(fd), "", false };
    bool successful = session.channel.EstablishKey(HostType::Server);
    if (successful) _sessions.emplace(fd, std::move(session));
    return successful;
}
void SessionManager::DestroySession(int fd)
{
    if (Has(fd)) {
        _user_to_fd.erase(Get(fd).username);
        _sessions.erase(fd);
    }
}

bool SessionManager::Has(int fd)
{
    return _sessions.find(fd) != _sessions.end();
}
bool SessionManager::Has(std::string username)
{
    return _user_to_fd.find(username) != _user_to_fd.end() && Has(_user_to_fd[username]);
}

Session& SessionManager::Get(int fd)
{
    return _sessions.at(fd);
}
Session& SessionManager::Get(std::string username)
{
    return _sessions.at(_user_to_fd.at(username));
}

bool SessionManager::Authenticate(int fd, std::string username)
{
    bool success = false;
    
    if (Has(fd) && !Has(username))
    {
        _user_to_fd.emplace(username, fd);

        auto& session = Get(fd);
        session.username = username;
        session.authenticated = true;

        success = true;
    }

    return success;
}

bool SessionManager::Unauthenticate(int fd)
{
    bool success = false;

    if (Has(fd))
    {
        auto& session = Get(fd);
        if (session.authenticated)
        {
            session.username = "";
            session.authenticated = false;
            success = true;
        }
    }

    return success;
}