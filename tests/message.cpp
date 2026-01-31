#include "Message.h"
#include <iostream>

int main()
{
    Message m;
    m.Set("type", "chat");
    m.Set("to", "alice");
    m.Set("from", "bob");
    m.Set("content", "You're really cool.");
    std::cout << "m     " << m.ToString() << std::endl;
    
    Message n;
    // m.Serialize();
    n.Deserialize(m.Serialize());
    std::cout << "n     " << n.ToString() << std::endl;

    return 0;
}