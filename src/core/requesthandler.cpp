#include "requesthandler.h"

#include "client.h"
#include "clients.h"
#include "router.h"

#include <cstdlib>
#include <unistd.h>

using namespace grackle;

RequestHandler::RequestHandler(std::shared_ptr<Clients> &clients) : m_clients(clients), m_router(new Router)
{

}

unsigned int RequestHandler::headerToInt(const char *header)
{
	auto ret = std::atoi(header);
	if (ret > 0) {
		return ret;
    } else {
	    return 0;
    }
}

int RequestHandler::readHeader(const int index)
{
    const int bytesrd = read(m_clients->getPollClients()[index].fd,
                             m_clients->getClients()[index].m_header + m_clients->getClients()[index].m_headerBytesRd,
                             m_clients->getClients()[index].m_HEADERSIZE - m_clients->getClients()[index].m_headerBytesRd);
    if (bytesrd < 0) {
        if ((errno == EAGAIN) || (errno == EINTR)) {
            return m_HEADERNOTDONE;
        }

        return m_HEADERREADERROR;
    } else if (bytesrd == 0) {
        return m_CLIENTCLOSEDCONN;
    }

    m_clients->getClients()[index].m_headerBytesRd += bytesrd;
    if (m_clients->getClients()[index].m_headerBytesRd == m_clients->getClients()[index].m_HEADERSIZE) {
        m_clients->getClients()[index].m_headerDone = true;
        m_clients->getClients()[index].m_bodyLength = headerToInt(m_clients->getClients()[index].m_header);
        if (m_clients->getClients()[index].m_bodyLength > m_clients->getClients()[index].m_BODYSIZE) {
            return m_HEADERREADERROR; // Stop the body buffer from overflowing
        }

        return m_HEADERDONE;
    }

    return m_HEADERNOTDONE;
}

int RequestHandler::readBody(const int index)
{
    const auto bytesrd = read(m_clients->getPollClients()[index].fd,
                              m_clients->getClients()[index].m_body + m_clients->getClients()[index].m_bodyBytesRd,
                              m_clients->getClients()[index].m_bodyLength - m_clients->getClients()[index].m_bodyBytesRd);
    if (bytesrd < 0) {
        if ((errno == EAGAIN) || (errno == EINTR)) {
            return m_BODYNOTDONE; 
        }

        return m_BODYREADERROR;
    }

    if (bytesrd == 0) {
        return m_CLIENTCLOSEDCONN;
    }

    m_clients->getClients()[index].m_bodyBytesRd += bytesrd;
    if (m_clients->getClients()[index].m_bodyBytesRd < m_clients->getClients()[index].m_bodyLength) {
        return m_BODYNOTDONE;
    }
    
    return m_BODYDONE;
}

void RequestHandler::doReadBody(const int index)
{
    auto status = readBody(index);
    switch (status) {
    case m_BODYDONE:
        // perform the request
        // then reset the client's buffers
        m_router->route(index);
        m_clients->getClients()[index].reset();
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

void RequestHandler::handleRequest(const int index)
{
    if (!m_clients->getClients()[index].m_readMutex.try_lock()) {
        return;
    }

    if (m_clients->getPollClients()[index].fd == -1) {
        m_clients->getClients()[index].m_readMutex.unlock();
        return;
    }

    if (m_clients->getClients()[index].m_headerDone == true) {
        doReadBody(index);
    } else {
        doReadHeader(index);
    }

    m_clients->getClients()[index].m_readMutex.unlock();
}

std::unique_ptr<Router> &RequestHandler::getRouter()
{
    return m_router;
}