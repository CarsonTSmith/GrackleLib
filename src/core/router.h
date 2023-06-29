#ifndef _ROUTER_H_
#define _ROUTER_H_

#include <functional>
#include <memory>
#include <string>
#include <unordered_map>

namespace grackle {

class Clients;

class Router {

private:
    std::unordered_map<std::string, std::function<std::string(std::string &)>> m_routeTable;
    std::shared_ptr<Clients> m_clients;

public:
    Router(const std::shared_ptr<Clients> &clients);
    bool addRoute(const std::string &path,
                  const std::function<std::string(std::string &)> &callback);
    std::pair<bool, std::string> route(const int index);

}; /* class Router */

} /* namespace grackle */


#endif /* _ROUTER_H_ */