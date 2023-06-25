#ifndef _REQUEST_HANDLER_H_
#define _REQUEST_HANDLER_H_

#include "clients.h"

#include <functional>
#include <memory>
#include <unordered_map>

namespace grackle {

class RequestHandler {

private:
    // TODO: Make only 1 set of ret values
    static constexpr int m_HEADERDONE       = 0;
    static constexpr int m_HEADERNOTDONE    = -1;
    static constexpr int m_HEADERREADERROR  = -2;

    static constexpr int m_BODYDONE         = 0;
    static constexpr int m_BODYNOTDONE      = -1;
    static constexpr int m_BODYREADERROR    = -2;

    static constexpr int m_CLIENTCLOSEDCONN = 1;

    std::shared_ptr<Clients> m_clients;
    std::unordered_map<std::string, std::function<void()>> m_router; // TODO: maybe move this to a seperate class

    void doReadHeader(const int index);
    void doReadBody(const int index);
    int readBody(const int index);
    int readHeader(const int index);
    unsigned int headerToInt(const char *header);
    void route(const int index); // TODO: maybe move this to a seperate class

public:
    void handleRequest(const int index);

}; /* class RequestHandler */

} /* namespace grackle */

#endif /* _REQUEST_HANDLER_H_ */