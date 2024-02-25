#include <grackle.h> // GrackleServer

#include <iostream> // cout
#include <string> // string
#include <utility> // pair
#include <vector> // vector

static std::string hello(std::string &body)
{
    return "Hello, World!";
}

int main(int argc, char *argv[])
{
    grackle::GrackleServer::Endpoints endpoints;
    endpoints.emplace_back(std::make_pair("/hello", hello));
    grackle::GrackleServer server(endpoints);

    std::cout << "Server Running" << std::endl;
    std::cout << "Press enter to exit" << std::endl;
    std::cin.get();
    std::cout << "Exiting" << std::endl;
    return 0;
}
