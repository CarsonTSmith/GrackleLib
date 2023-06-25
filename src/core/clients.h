#ifndef _CLIENTS_H_
#define _CLIENTS_H_

#include "client.h"

#include <atomic>
#include <poll.h>
#include <vector>

namespace grackle {

class Clients {

private:
    int m_maxClients = 64;
    std::atomic<int> m_numClients = { 0 };
    std::vector<pollfd> m_pollClients;
    std::vector<Client> m_clients;

    void init();

public:
    Clients();
    int  add(const int clientfd);
    void reset(const int index);
    void setMaxClients(const int maxClients);
}; /* class Clients */

} /* namespace grackle */


#endif /* _CLIENTS_H_ */