#include <grackle.h>

#include <iostream>
#include <string>

static std::string hello(std::string &body)
{
    return "Hello, World!";
}

int main(int argc, char *argv[])
{
    grackle::GrackleServer server;

    server.setPort(42125);
    server.setMaxClients(100);
    server.addEndpoint("/hello", hello);
    //server.runAsDaemon(); // optional
    server.start();

    std::cout << "Server Running" << std::endl;
    std::cout << "Press any key to exit" << std::endl;
    std::cin.get();
    std::cout << "Exiting" << std::endl;
    return 0;
}
