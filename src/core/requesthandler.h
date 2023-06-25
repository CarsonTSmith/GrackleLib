#ifndef _REQUEST_HANDLER_H_
#define _REQUEST_HANDLER_H_

#include "clients.h"

#include <memory>

namespace grackle {

class RequestHandler {

private:
    static constexpr int m_headerDone       = 0;
    static constexpr int m_headerNotDone    = -1;
    static constexpr int m_headerReadError  = -2;

    static constexpr int m_bodyDone         = 0;
    static constexpr int m_bodyNotDone      = -1;
    static constexpr int m_bodyReadError    = -2;

    static constexpr int m_clientClosedConn = 1;

    std::shared_ptr<Clients> m_clients;

    void doReadHeader(const int index);
    void doReadBody(const int index);
    int readBody(const int index);
    int readHeader(const int index);
    unsigned int headerToInt(const char *header);

public:
    void handleRequest(const int index);

}; /* class RequestHandler */

} /* namespace grackle */

#endif /* _REQUEST_HANDLER_H_ */