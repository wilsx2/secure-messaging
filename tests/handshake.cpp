#include "Shared/TestUtils.hpp"

int main()
{
    SetTestLog();
    TestServer ts;

    {
        Client client;
        client.Connect();
        AssertResponse(client, Register(usernames[0], passwords[0]), 0);
        AssertResponse(client, Login(usernames[0], passwords[0]), 0);
        AssertResponse(client, SendChat(usernames[0], "I love you."), 0);
        assert(client.GetUnread().size() == 1);
    }

    return 0;
}