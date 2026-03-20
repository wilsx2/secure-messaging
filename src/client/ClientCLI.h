#ifndef CLIENT_CLI_H
#define CLIENT_CLI_H

#include "client/ClientSession.h"
#include <memory>
#include <thread>

class ClientCLI
{
    private:
    ClientSession _session;
    std::unique_ptr<std::thread> _recv_thread;
    std::unique_ptr<std::thread> _send_thread;

    void ReceiveLoop();
    void SendLoop();

    public:
    ClientCLI() = default;
    ~ClientCLI() = default;
    void Run();
};

#endif