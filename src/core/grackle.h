#ifndef _GRACKLE_H_
#define _GRACKLE_H_

#include <functional>
#include <memory>
#include <string>

namespace grackle {

class GrackleServer {

private:
    class GrackleServerImpl;
    std::unique_ptr<GrackleServerImpl> m_impl;

public:
    // Constructor
    GrackleServer();

    // Destructor
    ~GrackleServer();

    // Starts the server
    bool start();

    // Sets the port number that the tcp socket will bind to
    void setPort(const int port);

    // Adds an endpoint and a callback to the RequestRouter
    bool addEndpoint(const std::string &path, const std::function<void()> &callback);

}; /* class GrackleServer */

} /* namespace grackle */


#endif /* _GRACKLE_H_ */