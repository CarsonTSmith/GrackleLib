#ifndef _REQUEST_HANDLER_H_
#define _REQUEST_HANDLER_H_

#include <functional>
#include <memory>
#include <unordered_map>

namespace grackle {

class Clients;
class Responder;
class Router;

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

    std::shared_ptr<Clients>   m_clients;
    std::unique_ptr<Responder> m_responder;
    std::unique_ptr<Router>    m_router;

    void doReadHeader(const int index);
    void doReadBody(const int index);
    int  readBody(const int index);
    int  readHeader(const int index);
    int  headerToInt(const char *header);
    void doRoute(const int index);

public:
    RequestHandler(const std::shared_ptr<Clients> &clients);
    void handleRequest(const int index);
    std::unique_ptr<Router> &getRouter();

}; /* class RequestHandler */

} /* namespace grackle */

#endif /* _REQUEST_HANDLER_H_ */