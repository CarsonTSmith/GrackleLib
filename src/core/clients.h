#ifndef _CLIENTS_H_
#define _CLIENTS_H_

#include <atomic>

namespace grackle {

class Clients {

private:
    std::atomic<int> numClients = { 0 };

}; /* class Clients */

} /* namespace grackle */


#endif _CLIENTS_H_