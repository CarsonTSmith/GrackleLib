#include "tcpserver.h"

#include <errno.h>
#include <fcntl.h>
#include <iostream>
#include <sys/socket.h>
#include <thread>
#include <unistd.h>

using namespace grackle;

TcpServer::TcpServer(std::shared_ptr<Clients> &clients) : m_clients(clients)
{

}

void TcpServer::doAccept(struct sockaddr_in *addr)
{
    socklen_t addrsz = sizeof(*addr);
    int clientfd, flags, index;

    while (1) {
        clientfd = accept(m_sockfd, (struct sockaddr *)addr, &addrsz);
        if (clientfd < 0) {
            fprintf(stderr, "tcp_server::do_accept() failed");
            exit(-1);
        }

        flags = fcntl(clientfd, F_GETFL, 0);
        fcntl(clientfd, F_SETFL, flags | O_NONBLOCK);

        //index = m_clients->add(clientfd);
        if (index < 0) {
            close(clientfd);
        }
    }
}

void TcpServer::doPoll()
{
    
}

int TcpServer::doStart(struct sockaddr_in *addr)
{
    int opt = 1;
    

    m_sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (m_sockfd < 0) {
        std::cerr << "Tcp Socket failed with errno " << errno << std::endl;
        return errno;
    }

    if (setsockopt(m_sockfd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0) {
        std::cerr << "Tcp setsockopt failed with errno " << errno << std::endl;
        return errno;
    }

    addr->sin_family = AF_INET;
    addr->sin_addr.s_addr = INADDR_ANY;
    addr->sin_port = htons(m_port);

    if (bind(m_sockfd, (struct sockaddr *)addr, sizeof(*addr)) < 0) {
        std::cerr << "Tcp bind failed with errno " << errno << std::endl;
        return errno;
    }

    if (listen(m_sockfd, 32) < 0) {
        std::cerr << "Tcp Listen failed with errno " << errno << std::endl;
        return errno;
    }

    return 0;
}

int TcpServer::start()
{
    struct sockaddr_in addr;
    auto status = doStart(&addr);
    if (status != 0) {
        return status;
    }

    // These two threads will be in infinite loops either accepting incoming
    // Tcp connections or polling clients
    std::thread acceptThread(&TcpServer::doAccept, this, &addr);
    acceptThread.detach();

    std::thread pollThread(&TcpServer::doPoll, this);
    pollThread.detach();

    return 0;
}
