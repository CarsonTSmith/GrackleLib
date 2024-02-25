#include "requesthandler.h"

#include "client.h"
#include "clients.h"
#include "responder.h"
#include "router.h"

#include <iostream>
#include <cstdlib>
#include <unistd.h>

using namespace grackle;

RequestHandler::RequestHandler(const std::shared_ptr<Clients> &clients) :
    m_clients(clients),
    m_responder(new Responder(clients)),
    m_router(new Router(clients))
{

}

int RequestHandler::headerToInt(const char *header)
{
	auto ret = std::atoi(header);
    std::cout << "Header is " << ret << std::endl;
	if (ret > 0) {
		return ret;
    } else {
	    return 0;
    }
}

int RequestHandler::readHeader(const int index)
{
    auto &client = m_clients->getClients()[index];
    const int bytesrd = read(m_clients->getPollClients()[index].fd,
                             client.m_header + client.m_headerBytesRd,
                             client.m_HEADERSIZE - client.m_headerBytesRd);
    if (bytesrd < 0) {
        if ((errno == EAGAIN) || (errno == EINTR)) {
            return m_HEADERNOTDONE;
        }

        return m_HEADERREADERROR;
    } else if (bytesrd == 0) {
        return m_CLIENTCLOSEDCONN;
    }

    client.m_headerBytesRd += bytesrd;
    if (client.m_headerBytesRd == client.m_HEADERSIZE) {
        client.m_headerDone = true;
        client.m_bodyLength = headerToInt(client.m_header);
        if (client.m_bodyLength > client.m_BODYSIZE) {
            return m_HEADERREADERROR; // Stop the body buffer from overflowing
        }

        return m_HEADERDONE;
    }

    return m_HEADERNOTDONE;
}

int RequestHandler::readBody(const int index)
{
    auto &client = m_clients->getClients()[index];
    const auto bytesrd = read(m_clients->getPollClients()[index].fd,
                              client.m_body + client.m_bodyBytesRd,
                              client.m_bodyLength - client.m_bodyBytesRd);
    if (bytesrd < 0) {
        if ((errno == EAGAIN) || (errno == EINTR)) {
            return m_BODYNOTDONE; 
        }

        return m_BODYREADERROR;
    }

    if (bytesrd == 0) {
        return m_CLIENTCLOSEDCONN;
    }

    client.m_bodyBytesRd += bytesrd;
    if (client.m_bodyBytesRd < client.m_bodyLength) {
        return m_BODYNOTDONE;
    }
    
    return m_BODYDONE;
}

void RequestHandler::doReadBody(const int index)
{
    auto status = readBody(index);
    switch (status) {
    case m_BODYDONE:
        doRoute(index);
        break;
    case m_BODYNOTDONE:
        break; // go back to polling
    case m_BODYREADERROR:
    case m_CLIENTCLOSEDCONN:
    default:
        m_clients->reset(index);
        break;
    }
}

void RequestHandler::doReadHeader(const int index)
{
    auto status = readHeader(index);
    switch (status) {
    case m_HEADERDONE:
        doReadBody(index);
        break;
    case m_HEADERNOTDONE:
        break; // go back to polling
    case m_HEADERREADERROR:
    case m_CLIENTCLOSEDCONN:
    default:
        m_clients->reset(index);
        break;
    }
}

void RequestHandler::doRoute(const int index)
{
    std::cout << "doRoute" << std::endl;
    auto result = m_router->route(index);
    if (result.first) {
        m_responder->sendToOne(index, result.second);
    }

    m_clients->getClients()[index].reset();
}

void RequestHandler::handleRequest(const int index)
{
    std::cout << "handleRequest" << std::endl;
    auto &client = m_clients->getClients()[index];
    if (!client.m_readMutex.try_lock()) {
        return;
    }

    if (m_clients->getPollClients()[index].fd == -1) {
        client.m_readMutex.unlock();
        return;
    }

    if (client.m_headerDone == true) {
        doReadBody(index);
    } else {
        doReadHeader(index);
    }

    client.m_readMutex.unlock();
}

std::unique_ptr<Router> &RequestHandler::getRouter()
{
    return m_router;
}
