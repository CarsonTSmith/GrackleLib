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
    std::vector<grackle::GrackleServer::Endpoint> endpoints;
    endpoints.emplace_back(std::make_pair("/hello", hello));
    try {
        grackle::GrackleServer server(endpoints, 42125, 64, false);
    } catch (const std::exception &e) {
        std::cerr << "Grackle Server failed to initialize. " << e.what() << std::endl;
        return 1;
    }

    std::cout << "Server Running" << std::endl;
    std::cout << "Press enter to exit" << std::endl;
    std::cin.get();
    std::cout << "Exiting" << std::endl;
    return 0;
}
