#ifndef _GRACKLE_H_
#define _GRACKLE_H_

#include <functional> // function
#include <memory> // unique_ptr
#include <string> // string
#include <utility> // pair
#include <vector> // vector

namespace grackle {

class GrackleServer {

public:
    typedef std::function<std::string(std::string &)> Callback;
    typedef std::pair<std::string, Callback> Endpoint;

private:
    class Impl;
    std::unique_ptr<Impl> m_impl;

public:
    GrackleServer(const std::vector<Endpoint> &endpoints,
                  const int port = 42125,
                  const int maxClients = 64,
                  const bool runAsDaemon = true);

    ~GrackleServer();

    GrackleServer(const GrackleServer &) = delete;
    GrackleServer(GrackleServer &&) = delete;
    GrackleServer &operator=(const GrackleServer &) = delete;
    GrackleServer &operator=(GrackleServer &&) = delete;
}; /* class GrackleServer */

} /* namespace grackle */

#endif /* _GRACKLE_H_ */