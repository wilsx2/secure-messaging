#include "Network/Message.h"
#include "Network/Serialization.h"
#include "Shared/Messages.h"
#include <cassert>

int main()
{
    std::vector<uint8_t> bytes (1024);

    {
        Login a ("Fizz", "Buzz");    
        Login b;
        assert(a.Serialize(bytes) && b.Deserialize(bytes) && a.username == b.username && a.password == b.password);
    }
    {
        Register a ("Fizz", "Buzz");    
        Register b;
        assert(a.Serialize(bytes) && b.Deserialize(bytes) && a.username == b.username && a.password == b.password);
    }
    {
        Logout a;
        assert(a.Serialize(bytes) && a.Deserialize(bytes));
    }
    {
        DeleteAccount a;
        assert(a.Serialize(bytes) && a.Deserialize(bytes));
    }
    {
        ChangePassword a ("Fizz");    
        ChangePassword b;
        assert(a.Serialize(bytes) && b.Deserialize(bytes) && a.new_password == b.new_password);
    }
    {
        ActiveUsers a;
        assert(a.Serialize(bytes) && a.Deserialize(bytes));
    }
    {
        SendChat a ("Fizz", "Buzz");    
        SendChat b;
        assert(a.Serialize(bytes) && b.Deserialize(bytes) && a.to == b.to && a.content == b.content);
    }
    {
        ReceiveChat a ("Fizz", "Buzz");    
        ReceiveChat b;
        assert(a.Serialize(bytes) && b.Deserialize(bytes) && a.from == b.from && a.content == b.content);
    }
    {
        Success a;
        assert(a.Serialize(bytes) && a.Deserialize(bytes));
    }
    {
        Failure a ("Fizz");    
        Failure b;
        assert(a.Serialize(bytes) && b.Deserialize(bytes) && a.what == b.what);
    }
    {
        std::vector<std::string> strings {"Snap", "Crackle", "Pop"};
        StringList a (std::move(strings));    
        StringList b;

        assert(a.Serialize(bytes) && b.Deserialize(bytes) && a.value == b.value);
    }

    return 0;
}