#include "grackle.h"

#include "client.h" // Client
#include "clients.h" // Clients
#include "responder.h" // Responder
#include "requesthandler.h" // RequestHandler
#include "router.h" // Router

#include <atomic> // atomic
#include <fcntl.h> // fcntl
#include <iostream> // cerr
#include <memory> // shared_ptr, unique_ptr
#include <netinet/in.h> // struct sockaddr_in
#include <stdexcept> // runtime_error
#include <thread> // thread
#include <unistd.h> // close
#include <vector> // vector

using namespace grackle;

class GrackleServer::Impl {

private:
    int                             m_port;
    int                             m_sockfd;
    struct sockaddr_in              m_addr;
    std::shared_ptr<Clients>        m_clients;
    std::unique_ptr<RequestHandler> m_requestHandler;
    std::thread                     m_acceptThread;
    std::thread                     m_pollThread;
    std::atomic<bool>               m_cancelThreads;
    bool                            m_isDaemon;

    // Threadpool    m_threadPool;

/*******************************************************************************
* Initializes the Tcp Server Socket
*******************************************************************************/
int initializeTCPSocket()
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
*******************************************************************************/
void doAccept()
{
    socklen_t addrsz = sizeof(m_addr);
    int clientfd, flags;

    while (1) {
        if (m_cancelThreads) {
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

        std::cout << "Client added" << std::endl;
    }
}

/*******************************************************************************
* Infinite loop that polls clients and processes requests
*******************************************************************************/
void doPoll()
{
    int numFds;

    while (1) {
		numFds = poll(m_clients->getPollClients().data(),
                      m_clients->getMaxClients(),
                       50); // timeout so when new clients connect they are polled
        if (m_cancelThreads) {
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

bool validatePort(const int port)
{
    if ((port < 1025) || (port > 65535)) {
        std::cerr << "Port number is out of range" << std::endl;
        return false;
    }

    return true;
}

bool addEndpoints(const Endpoints &endpoints)
{
    for (const auto &endpoint: endpoints) {
        auto success = m_requestHandler->getRouter()->addRoute(endpoint.first, endpoint.second);
        if (!success) {
            std::cerr << "Failed to add endpoint for Path [" << endpoint.first << "]" << std::endl;
            return false;
        }
    }

    return true;
}

bool start()
{
    auto err = initializeTCPSocket();
    if (err) {
        std::cerr << "Tcp Server failed to start with errno [" << err << "]" << std::endl;
        return false;
    }

    displayBanner();
    displayServerInfo();

    // These two threads will be in infinite loops either accepting incoming
    // Tcp connections or polling clients
    m_acceptThread = std::thread(&Impl::doAccept, this);
    if (m_isDaemon) {
        doPoll();
    } else {
        m_pollThread = std::thread(&Impl::doPoll, this);
    }

    return true;
}

void displayServerInfo()
{
    std::cout << "GrackleServer listening on Port ["
              << m_port
              << "] with max clients ["
              << m_clients->getMaxClients()
              << "]"
              << std::endl;
}

void displayBanner()
{
    std::cout << "+======================================================================================+\n" \
                 "|  ____                    _     _        ____                               __ __ __  |\n" \
                 "| / ___| _ __  __ _   ___ | | __| |  ___ / ___|   ___  _ __ __   __ ___  _ __\\ \\ \\ \\ |\n" \
                 "|| |  _ | '__|/ _` | / __|| |/ /| | / _ \\___ \\  / _ \\| '__|\\ \\ / // _ \\| '__|\\ \\ \\ \\|\n" \
                 "|| |_| || |  | (_| || (__ |   < | ||  __/ ___) ||  __/| |    \\ V /|  __/| |   / // // /|\n" \
                 "| \\____||_|   \\__,_| \\___||_|\\_\\|_| \\___||____/  \\___||_|     \\_/  \\___||_|  /_//_//_/ |\n" \
                 "+======================================================================================+" << std::endl;
}



public:

Impl(const Endpoints &endpoints,
     const int port,
     const int maxClients,
     const bool runAsDaemon)
    try : m_port(port),
          m_clients(new Clients(maxClients)),
          m_requestHandler(new RequestHandler(m_clients)),
          m_isDaemon(runAsDaemon)
{
    bool success = validatePort(m_port) &&
                   addEndpoints(endpoints);
    if (!success) {
        std::cerr << "GrackleServer failed initialization. Terminating (throwing an exception)." << std::endl;
        throw std::runtime_error("GrackleServer failed initialization.");
    }

    success = start();
    if (!success) {
        std::cerr << "GrackleServer failed to start. Terminating (throwing an exception)." << std::endl;
        throw std::runtime_error("GrackleServer failed to start.");
    }
}
catch (...) {
    throw;
}

~Impl()
{
    m_cancelThreads = true;
    m_acceptThread.join();
    if (!m_isDaemon) {
        m_pollThread.join();
    }
}

}; /* class Impl */



GrackleServer::GrackleServer(const Endpoints &endpoints,
                             const int port,
                             const int maxClients,
                             const bool runAsDaemon)
    try : m_impl(new Impl(endpoints, port, maxClients, runAsDaemon))
{ 

}
catch(...) {
    throw;
}

GrackleServer::~GrackleServer() = default;