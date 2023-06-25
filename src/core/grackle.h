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
    // Constructor.
    GrackleServer();

    // Destructor.
    ~GrackleServer();

    // Starts the server.
    bool start();

    // Sets the port number that the Tcp socket will bind to.
    void setPort(const int port);

    // Sets the maximum number of Tcp clients that can be connected at once.
    void setMaxClients(const int maxClients);

    // Adds an endpoint and a callback to the Server.
    bool addEndpoint(const std::string &path,
                     const std::function<void(std::string &)> &callback);

}; /* class GrackleServer */

} /* namespace grackle */


#endif /* _GRACKLE_H_ */