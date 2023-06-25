#ifndef _ROUTER_H_
#define _ROUTER_H_

#include "clients.h"

#include <functional>
#include <memory>
#include <string>
#include <unordered_map>

namespace grackle {

class Router {

private:
    std::unordered_map<std::string, std::function<void(std::string &)>> m_routeTable;
    std::shared_ptr<Clients> m_clients;

public:
    bool addRoute(const std::string &path,
                  const std::function<void(std::string &)> &callback);
    void route(const int index);

}; /* class Router */

} /* namespace grackle */


#endif /* _ROUTER_H_ */