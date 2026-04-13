#include "Network/Message.h"
#include <iostream>

int main()
{
    Message m;
    m.Set("type", "chat");
    m.Set("to", "alice");
    m.Set("from", "bob");
    m.Set("content", "You're really cool.");
    
    Message n;
    n.Deserialize(m.Serialize());

    std::cout << "m     " << m.ToString() << std::endl;
    std::cout << "n     " << n.ToString() << std::endl;
    return !(m.ToString() == n.ToString());
}