#ifndef _CLIENTS_H_
#define _CLIENTS_H_

#include <atomic>
#include <poll.h>
#include <vector>

namespace grackle {

class Client;

class Clients {

private:
    int m_maxClients;
    std::atomic<int> m_numClients;
    std::vector<pollfd> m_pollClients;
    std::vector<Client> m_clients;

    void init();

public:
    Clients(const int maxClients = 64);

    Clients(const Clients &) = delete;
    Clients(Clients &&) = delete;
    Clients &operator=(const Clients &) = delete;
    Clients &operator=(Clients &&) = delete;

    ~Clients() = default;

    int  add(const int clientfd);
    void reset(const int index);

    int  getMaxClients();
    std::vector<pollfd> &getPollClients();
    std::vector<Client> &getClients();

}; /* class Clients */

} /* namespace grackle */


#endif /* _CLIENTS_H_ */