#include "Network/Message.h"
#include "Network/Serialization.h"
#include "Shared/Messages.h"
#include <cassert>

int main()
{
    std::vector<uint8_t> bytes (1024);

    {
        Login a (1, "Fizz", "Buzz");    
        Login b;
        assert(a.Serialize(bytes) && b.Deserialize(bytes) && a.id == b.id && a.username == b.username && a.password == b.password);
    }
    {
        Register a (2, "Fizz", "Buzz");    
        Register b;
        assert(a.Serialize(bytes) && b.Deserialize(bytes) && a.id == b.id && a.username == b.username && a.password == b.password);
    }
    {
        DeleteAccount a (3);    
        DeleteAccount b;
        assert(a.Serialize(bytes) && b.Deserialize(bytes) && a.id == b.id);
    }
    {
        ChangePassword a (4, "Fizz");    
        ChangePassword b;
        assert(a.Serialize(bytes) && b.Deserialize(bytes) && a.id == b.id && a.new_password == b.new_password);
    }
    {
        ActiveUsers a (5);    
        ActiveUsers b;
        assert(a.Serialize(bytes) && b.Deserialize(bytes) && a.id == b.id);
    }
    {
        SendChat a (6, "Fizz", "Buzz");    
        SendChat b;
        assert(a.Serialize(bytes) && b.Deserialize(bytes) && a.id == b.id && a.to == b.to && a.content == b.content);
    }
    {
        ReceiveChat a ("Fizz", "Buzz");    
        ReceiveChat b;
        assert(a.Serialize(bytes) && b.Deserialize(bytes) && a.from == b.from && a.content == b.content);
    }
    {
        Success a (7);    
        Success b;
        assert(a.Serialize(bytes) && b.Deserialize(bytes) && a.request_id == b.request_id);
    }
    {
        Failure a (8, "Fizz");    
        Failure b;
        assert(a.Serialize(bytes) && b.Deserialize(bytes) && a.request_id == b.request_id && a.what == b.what);
    }
    {
        std::vector<std::string> strings {"Snap", "Crackle", "Pop"};
        StringList a (9, std::move(strings));    
        StringList b;

        assert(a.Serialize(bytes) && b.Deserialize(bytes) && a.request_id == b.request_id && a.value == b.value);
    }

    return 0;
}