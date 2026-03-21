#include "shared/TestUtils.hpp"

int main()
{
    SetTestLog();
    Client client;

    {
        TestServer ts;

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