#include "clients.h"

#include "client.h"

#include <iostream> // cerr
#include <stdexcept> // runtime_error
#include <unistd.h> // close

using namespace grackle;

Clients::Clients(const int maxClients) : m_maxClients(maxClients), m_numClients(0)
{
    if (maxClients < 1) {
        std::cerr << "Max clients cannot be less than 1" << std::endl;
        throw std::runtime_error("Max clients cannot be less than 1");
    } else if (maxClients > 2147483647) {
        std::cerr << "Max clients cannot be more than 2,147,483,647" << std::endl;
        throw std::runtime_error("Max clients cannot be more than 2,147,483,647");
    }

    init();
}

void Clients::init()
{
    m_pollClients.resize(m_maxClients);
    m_clients.resize(m_maxClients);
    for (auto &pClient: m_pollClients) {
        pClient.fd     = -1;
        pClient.events = POLLIN;
    }
}

int Clients::add(const int clientfd)
{
    if (m_numClients > m_maxClients) {
        return -1;
    }

    for (auto &pClient: m_pollClients) {
        if (pClient.fd == -1) {
            pClient.fd = clientfd;
            m_numClients++;
            return 1;
        }
    }

    return -1;
}

void Clients::reset(const int index)
{
    if ((index < 0) || (index > m_maxClients)) {
        return;
    }

    close(m_pollClients[index].fd);
    m_pollClients[index].fd = -1;
    m_pollClients[index].revents = 0;
    m_clients[index].reset();
    m_numClients--;
}

int Clients::getMaxClients()
{
    return m_maxClients;
}

std::vector<pollfd> &Clients::getPollClients()
{
    return m_pollClients;
}

std::vector<Client> &Clients::getClients()
{
    return m_clients;
}