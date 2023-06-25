#ifndef _TCPSERVER_H_
#define _TCPSERVER_H_

#include "clients.h"
#include "requesthandler.h"

#include <memory>
#include <netinet/in.h>

namespace grackle {

class TcpServer {

private:
    int m_port = 42125;
    int m_sockfd;
    std::shared_ptr<Clients> m_clients;
    RequestHandler m_requestHandler;

    int  doStart(struct sockaddr_in *addr);
    void doAccept(struct sockaddr_in *addr);
    void doPoll();
    void process(int numFds);

public:
    TcpServer(std::shared_ptr<Clients> &clients);
    void setPort(const int port);
    int start();

}; /* class TcpSocket */

} /* namespace grackle */

#endif /* _TCPSERVER_H_ */