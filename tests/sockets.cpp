#include <iostream>
#include "TcpSocket.h"

int main(int argc, char* argv[])
{
    TcpSocket server (2000, INADDR_ANY);
    TcpSocket client (2001, INADDR_ANY);

}