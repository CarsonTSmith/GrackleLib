#include "router.h"

#include "client.h"
#include "clients.h"

#include <iostream> // cerr

using namespace grackle;

bool Router::addRoute(const std::string &path,
                      const std::function<std::string(std::string &)> &callback)
{
    if (path.empty()) {
        std::cerr << "Path cannot be empty" << std::endl;
        return false;
    }

    auto status = m_routeTable.insert(std::make_pair(path, callback));
    return status.second; // true or false where the insertion happened
}

std::pair<bool, std::string> Router::route(const int index)
{
    auto path = m_clients->getClients()[index].getPath();
    auto it = m_routeTable.find(path);
    if (it == m_routeTable.end()) {
        std::cerr << "Key [" << path << "] not found" << std::endl;
        return std::make_pair(false, "");
    } else {
        auto body = m_clients->getClients()[index].getBody();
        return std::make_pair(true, it->second(body));
    }
}