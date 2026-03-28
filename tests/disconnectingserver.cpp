#include "shared/TestUtils.hpp"

int main()
{
    SetTestLog();
    Client client;

    {
        TestServer ts;

        client.Connect();
    }

    return 0;
}