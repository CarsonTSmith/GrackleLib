#include "grackle.h"

#include "client.h" // Client
#include "clients.h" // Clients
#include "responder.h" // Responder
#include "requesthandler.h" // RequestHandler
#include "router.h" // Router

#include <atomic> // atomic
#include <fcntl.h> // fcntl
#include <iostream> // cerr
#include <memory> // shared_ptr
#include <netinet/in.h> // struct sockaddr_in
#include <thread> // thread
#include <unistd.h> // close

using namespace grackle;


/*******************************************************************************
* Implementation for GrackleServer
* GrackleServer will forward the calls to this class.
*
*******************************************************************************/
class GrackleServer::GrackleServerImpl {

/*******************************************************************************
* Private Member Variables
*
*******************************************************************************/
private:
    int                             m_port = 42125;
    int                             m_sockfd;
    struct sockaddr_in              m_addr;
    std::shared_ptr<Clients>        m_clients;
    std::unique_ptr<RequestHandler> m_requestHandler;
    std::thread                     m_acceptThread;
    std::thread                     m_pollThread;
    std::atomic<bool>               m_cancelThreads;

    // Threadpool    m_pool;




/*******************************************************************************
* Private Internal Functions
*
*******************************************************************************/

/*******************************************************************************
* Initializes the Tcp Socket
*
*******************************************************************************/
int doStart()
{
    m_sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (m_sockfd < 0) {
        std::cerr << "Tcp Socket failed with errno " << errno << std::endl;
        return errno;
    }

    auto flags = fcntl(m_sockfd, F_GETFL, 0);
    fcntl(m_sockfd, F_SETFL, flags | O_NONBLOCK);

    int opt = 1;
    if (setsockopt(m_sockfd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0) {
        std::cerr << "Tcp setsockopt failed with errno " << errno << std::endl;
        return errno;
    }

    m_addr.sin_family = AF_INET;
    m_addr.sin_addr.s_addr = INADDR_ANY;
    m_addr.sin_port = htons(m_port);

    if (bind(m_sockfd, (struct sockaddr *)&m_addr, sizeof(m_addr)) < 0) {
        std::cerr << "Tcp bind failed with errno " << errno << std::endl;
        return errno;
    }

    if (listen(m_sockfd, 32) < 0) {
        std::cerr << "Tcp Listen failed with errno " << errno << std::endl;
        return errno;
    }

    return 0;
}

/*******************************************************************************
* Infinite event loop that accepts incoming Tcp connections
*
*******************************************************************************/
void doAccept(std::atomic<bool> &cancel)
{
    socklen_t addrsz = sizeof(m_addr);
    int clientfd, flags;

    while (1) {
        if (cancel) {
            return;
        }

        clientfd = accept(m_sockfd, (struct sockaddr *)&m_addr, &addrsz);
        if (clientfd < 0) {
            if ((errno == EAGAIN) || (errno == EWOULDBLOCK)) {
                usleep(10000);
                continue;
            }

            std::cerr << "Tcp accept failed" << std::endl;
            continue;
        }

        flags = fcntl(clientfd, F_GETFL, 0);
        fcntl(clientfd, F_SETFL, flags | O_NONBLOCK);

        auto index = m_clients->add(clientfd);
        if (index < 0) {
            close(clientfd);
        }
    }
}

/*******************************************************************************
*
*
*******************************************************************************/
void doPoll(std::atomic<bool> &cancel)
{
    int numFds;

    while (1) {
		numFds = poll(m_clients->getPollClients().data(),
                      m_clients->getMaxClients(),
                       50); // timeout so when new clients connect they are polled
        if (cancel) {
            return;
        } else if (numFds == 0) {
            continue;
        } else if (numFds > 0) {
			process(numFds);
		} else if (numFds < 0) { // poll error
			std::cerr << "Poll error with errno " << errno << std::endl;
			continue;
		}
	}
}

/*******************************************************************************
* Grabs all the Sockets/Clients that had an event occur and processes them
*
*******************************************************************************/
void process(int numFds)
{
    for (int i = 0; (numFds > 0) && (i < m_clients->getMaxClients()); ++i) {
        if (m_clients->getPollClients()[i].revents & POLLIN) {
            m_requestHandler->handleRequest(i);
            numFds--;
        }
    }
}



/*******************************************************************************
* Public Member Functions
*
*******************************************************************************/
public:



/*******************************************************************************
* Constructor
*
*******************************************************************************/
GrackleServerImpl() : m_clients(new Clients),
                      m_requestHandler(new RequestHandler(m_clients))
{

}


~GrackleServerImpl()
{
    m_cancelThreads = true;
    m_acceptThread.join();
    m_pollThread.join();
} // join the accept and poll threads here

/*******************************************************************************
* Start the server
*
*******************************************************************************/
bool start()
{
    auto status = doStart();
    if (status != 0) {
        std::cerr << "Tcp Server failed to start" << std::endl;
        return false;
    }

    // These two threads will be in infinite loops either accepting incoming
    // Tcp connections or polling clients
    m_acceptThread = std::thread(&GrackleServerImpl::doAccept, this, std::ref(m_cancelThreads));
    m_pollThread   = std::thread(&GrackleServerImpl::doPoll, this, std::ref(m_cancelThreads));

    return true;
}

/*******************************************************************************
*
*
*******************************************************************************/
void setPort(const int port)
{
    if ((port < 1) || (port > 65535)) {
        std::cerr << "Port number is out of range" << std::endl;
        return;
    }

    m_port = port;
}

/*******************************************************************************
* Set the maximum simultaneous Tcp connections
*
*******************************************************************************/
void setMaxClients(const int maxClients)
{
    if (maxClients < 1) {
        std::cerr << "Max clients can not be less than 1" << std::endl;
        return;
    }

    m_clients->setMaxClients(maxClients);
}

/*******************************************************************************
* Add an endpoint to the server
*
*******************************************************************************/
bool addEndpoint(const std::string &path, const std::function<std::string(std::string &)> &callback)
{
    return m_requestHandler->getRouter()->addRoute(path, callback);
    return true;
}

}; /* class GrackleServerImpl */










/*******************************************************************************
* GrackleServer
* This class will forward its calls to GrackleServerImpl.
* There will be no logic in these methods only forwarding.
*
*******************************************************************************/


/*******************************************************************************
* Constructor
*
*******************************************************************************/
GrackleServer::GrackleServer() : m_impl(new GrackleServerImpl)
{

}

/*******************************************************************************
* Destructor - Stops the Accept and Poll threads and joins them
*
*******************************************************************************/
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
* Sets the port that the server will bind to.
*
*******************************************************************************/
void GrackleServer::setPort(const int port)
{
    m_impl->setPort(port);
}

/*******************************************************************************
* Sets the maximum number of TCP clients that can be connected at once.
*
*******************************************************************************/
void GrackleServer::setMaxClients(const int maxClients)
{
    m_impl->setMaxClients(maxClients);
}

/*******************************************************************************
* Add an endpoint to the server.
*
* @parameter path: parameter will be the value that is in the json body with key "path"
* @parameter callback: callback defined by the library user
*
* @return : will return false if an endpoint with that path already exists
* eg. {"path":"/example/path"}
*
*******************************************************************************/
bool GrackleServer::addEndpoint(const std::string &path,
                                const std::function<std::string(std::string &)> &callback)
{
    return m_impl->addEndpoint(path, callback);
}
