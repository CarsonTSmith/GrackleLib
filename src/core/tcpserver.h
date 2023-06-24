#ifndef _TCPSERVER_H_
#define _TCPSERVER_H_

#include <memory>
#include <netinet/in.h>

namespace grackle {

class TcpServer {

private:
    int         m_port;
    int         m_sockfd;
    //std::shared_ptr<Clients> m_clients;

    int  doStart(struct sockaddr_in *addr);
    void doAccept(struct sockaddr_in *addr);
    void doPoll();

public:
    TcpServer(std::shared_ptr<Clients> &clients);
    int start();

}; /* class TcpSocket */

} /* namespace grackle */

#endif /* _TCPSERVER_H_ */