#include "grackle.h"

#include "tcpserver.h"

#include <iostream> // cerr
#include <memory> // shared_ptr

using namespace grackle;


/*******************************************************************************
* Declaration and implementation for GrackleServerImpl.
* GrackleServer will forward the calls to this class.
*
*******************************************************************************/
class GrackleServer::GrackleServerImpl {

private:
    // RequestRouter m_router;
    // Threadpool    m_pool;
    TcpServer     m_tcpServer;
    std::shared_ptr<Clients> m_clients;
    
    
public:
    GrackleServerImpl() : m_clients(new Clients), m_tcpServer(m_clients)
    {

    }

    bool start()
    {
        auto status = m_tcpServer.start();
        if (status != 0) {
            std::cerr << "Tcp Server failed to start" << std::endl;
            return false;
        }

        return true;
    }

    void setPort(const int port)
    {
        if ((port < 1) || (port > 65535)) {
            std::cerr << "Port number is out of range" << std::endl;
            return;
        }

        //m_tcpServer.setPort(port);
    }

    bool addEndpoint(const std::string &path, const std::function<void()> &callback)
    {
        // m_router.addEndpoint(path, callback);
        return true;
    }
};



/*******************************************************************************
* GrackleServer
* This class will forward its calls to GrackleServerImpl.
* There will be no logic in these methods only forwarding.
*
*******************************************************************************/
GrackleServer::GrackleServer() : m_impl(new GrackleServerImpl)
{

}

GrackleServer::~GrackleServer() = default;


/*******************************************************************************
* Starts the server and begins listening on the port set by setPort().
* Starts the accept() and poll() event loops.
*
*******************************************************************************/
bool GrackleServer::start()
{
    return m_impl->start();
}

/*******************************************************************************
* Set the port that the server will bind to.
*
*******************************************************************************/
void GrackleServer::setPort(const int port)
{
    m_impl->setPort(port);
}

/*******************************************************************************
* Add an endpoint to the server.
*
* @parameter path: parameter will be the value that is in the json body with key "path"
* @parameter callback: callback defined by the library user
*
* @return : will return false if an endpoint with that path already exists
* eg. {"path":"/example/path"}

*******************************************************************************/
bool GrackleServer::addEndpoint(const std::string &path,
                                const std::function<void()> &callback)
{
    return m_impl->addEndpoint(path, callback);
}
