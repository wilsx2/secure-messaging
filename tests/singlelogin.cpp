#include "shared/TestUtils.hpp"

int main()
{
    SetTestLog();
    TestServer ts;

    {
        Client a;
        a.Connect();

        {
            Client b;
            b.Connect();
            SendCommand(b, "register " + accounts(0));
            AwaitType(b, "registered");
            SendCommand(b, "login " + accounts(0));
            AwaitType(b, "logged in");
            SendCommand(a, "login " + accounts(0));
            AwaitType(a, "error");
        }

        SendCommand(a, "login " + accounts(0));
        AwaitType(a, "logged in");
    }
    return 0;
}