#include "Server/SessionManager.h"

bool SessionManager::Create(int fd)
{
    Session session { SecureChannel(fd), "", false };

    bool success = session.channel.EstablishKey(HostType::Server);
    if (success)
        _sessions.emplace(fd, std::move(session));

    return success;
}
bool SessionManager::Destroy(int fd)
{
    bool success = false;

    if (IsEstablished(fd) && IsAuthenticated(fd)) {
        _user_to_fd.erase(GetUsername(fd));
        _sessions.erase(fd);
        success = true;
    }

    return success;
}

bool SessionManager::IsEstablished(int fd) const
{
    return _sessions.find(fd) != _sessions.end();
}
bool SessionManager::IsEstablished(const std::string& username) const
{
    return _user_to_fd.find(username) != _user_to_fd.end() && IsEstablished(_user_to_fd.at(username));
}

SecureChannel& SessionManager::GetChannel(int fd)
{
    return _sessions.at(fd).channel;
}
SecureChannel& SessionManager::GetChannel(const std::string& username)
{
    return _sessions.at(_user_to_fd.at(username)).channel;
}
const std::string& SessionManager::GetUsername(int fd)  const
{
    return _sessions.at(fd).username;
}
bool SessionManager::IsAuthenticated(int fd) const
{
    return _sessions.at(fd).authenticated;
}

bool SessionManager::Authenticate(int fd, const std::string& username)
{
    bool success = false;
    
    if (IsEstablished(fd) && !IsEstablished(username))
    {
        _user_to_fd.emplace(username, fd);

        auto& session = _sessions.at(fd);
        session.username = username;
        session.authenticated = true;

        success = true;
    }

    return success;
}

bool SessionManager::Unauthenticate(int fd)
{
    bool success = false;

    if (IsEstablished(fd))
    {
        auto& session = _sessions.at(fd);
        if (session.authenticated)
        {
            session.username = "";
            session.authenticated = false;
            success = true;
        }
    }

    return success;
}