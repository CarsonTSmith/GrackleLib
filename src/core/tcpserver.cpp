#include "tcpserver.h"

#include <errno.h>
#include <fcntl.h>
#include <iostream>
#include <poll.h>
#include <sys/socket.h>
#include <thread>
#include <unistd.h>

using namespace grackle;

TcpServer::TcpServer(std::shared_ptr<Clients> &clients) : m_clients(clients)
{

}

void TcpServer::process(int numFds)
{
    /* for (int i = 0; (num_fds > 0) && (i < clients::MAX_CLIENTS); ++i) {
        if (clients.p_clients[i].revents & POLLIN) {
            while (1) {
                if (threadpool::g_threadpool.get_q_size() > threadpool::MAX_QUEUE_SIZE) {
                    {
                        std::unique_lock lk(threadpool::mutex);
                        bool status = threadpool::cv.wait_for(lk, std::chrono::milliseconds(5),
                                                []{return (threadpool::tasks_in_queue < threadpool::MAX_QUEUE_SIZE);});
                        if (status) {
                            break;
                        }
                    }
                } else {
                    break;
                }
            }

            threadpool::g_threadpool.push(request::handle_request, i);
            num_fds--;
        }
    } */
}

void TcpServer::doAccept(struct sockaddr_in *addr)
{
    socklen_t addrsz = sizeof(*addr);
    int clientfd, flags;

    while (1) {
        clientfd = accept(m_sockfd, (struct sockaddr *)addr, &addrsz);
        if (clientfd < 0) {
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

void TcpServer::doPoll()
{
    /* int num_fds; */

    while (1) {
		/* num_fds = poll(clients.p_clients,
                       clients::MAX_CLIENTS,
                       50); // timeout so when new clients connect they are polled
		if (num_fds > 0) {
			process(clients, num_fds);
		} else if (num_fds < 0) { // poll error
			fprintf(stderr, "rd_from_clients() poll error\n%d", errno);
			exit(errno);
		} else if (num_fds == 0) { // no data sent yet, poll again
			continue;
		} */
	}
}

int TcpServer::doStart(struct sockaddr_in *addr)
{
    m_sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (m_sockfd < 0) {
        std::cerr << "Tcp Socket failed with errno " << errno << std::endl;
        return errno;
    }

    int opt = 1;
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
    // TODO: These will need to be killed when the GrackleServer object destructs
    std::thread acceptThread(&TcpServer::doAccept, this, &addr);
    acceptThread.detach();

    std::thread pollThread(&TcpServer::doPoll, this);
    pollThread.detach();

    return 0;
}

void TcpServer::setPort(const int port)
{
    if ((port < 1) || (port > 65535)) {
        return;
    }

    m_port = port;
}
