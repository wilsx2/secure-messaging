#pragma once

#include "Client/Client.h"
#include <memory>
#include <thread>

class ClientCLI
{
    private:
    Client _session;
    std::unique_ptr<std::thread> _recv_thread;
    std::unique_ptr<std::thread> _send_thread;

    void ReceiveLoop();
    void SendLoop();
    void PrintMessage(const Message& message);

    public:
    ClientCLI() = default;
    ~ClientCLI() = default;
    void Run();
};