#include "Shared/TestUtils.hpp"

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
            AssertResponse(b, Register(usernames[0], passwords[0]), 0);
            AssertResponse(b, Login(usernames[0], passwords[0]), 0);
            AssertResponse(a, Login(usernames[0], passwords[0]), 1);
        }

        AssertResponse(a, Login(usernames[0], passwords[0]), 0);
    }
    return 0;
}