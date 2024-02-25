#include "router.h"

#include "client.h"
#include "clients.h"

#include <iostream> // cerr

using namespace grackle;

Router::Router(const std::shared_ptr<Clients> &clients) : m_clients(clients)
{

}

bool Router::addRoute(const std::string &path,
                      const std::function<std::string(std::string &)> &callback)
{
    if (path.empty()) {
        std::cerr << "Path cannot be empty" << std::endl;
        return false;
    }

    auto status = m_routeTable.insert(std::make_pair(path, callback));
    return status.second; // true or false whether the insertion happened
}

std::pair<bool, std::string> Router::route(const int index)
{
    // <path, body>
    auto pathAndBody = m_clients->getClients()[index].parse();
    auto it = m_routeTable.find(pathAndBody.first);
    if (it == m_routeTable.end()) {
        std::cerr << "Key [" << pathAndBody.first << "] not found" << std::endl;
        return std::make_pair(false, "");
    } else {
        return std::make_pair(true, it->second(pathAndBody.second));
    }
}