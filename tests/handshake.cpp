#include "Shared/TestUtils.hpp"

int main()
{
    SetTestLog();
    TestServer ts;

    {
        Client client;
        client.Connect();
        SendCommand(client, "register " + accounts(0));
        AwaitType(client, "registered");
        SendCommand(client, "login " + accounts(0));
        AwaitType(client, "logged in");
        SendCommand(client, "send " + usernames[0] + " love");
        AwaitType(client, "chat");
        AwaitType(client, "sent");
    }

    return 0;
}