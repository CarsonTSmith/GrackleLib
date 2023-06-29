#include <grackle.h>

#include <iostream>
#include <string>

static std::string echo(std::string &body)
{
    std::string str = "Hello, World!";
    return "00000013Hello, World!";
}

int main(int argc, char *argv[])
{
    grackle::GrackleServer server;

    server.setPort(42125);
    server.setMaxClients(100);
    server.addEndpoint("/echo", echo);
    server.start();

    std::cout << "Running" << std::endl;
    std::cin.get();
    std::cout << "Exiting" << std::endl;
    return 0;
}