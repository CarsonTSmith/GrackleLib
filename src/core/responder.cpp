#include "responder.h"

#include "client.h"
#include "clients.h"

#include <iostream> // cerr
#include <mutex> // lock_guard
#include <sys/socket.h> // send

using namespace grackle;

Responder::Responder(const std::shared_ptr<Clients> &clients) : m_clients(clients)
{

}

/*******************************************************************************
* @param index - Clients index of who is being sent the message
* @param msg   - The msg that is being sent
*
*******************************************************************************/
void Responder::sendToOne(const int index, const std::string &msg)
{
    std::cout << "Sending To One" << std::endl;
    std::cout << msg << std::endl;
    ssize_t result = 0, total = 0;

    if (msg.size() == 0) {
        std::cerr << "Attempting to send an empty msg" << std::endl;
        return;
    }

    std::lock_guard<std::mutex> lk(m_clients->getClients()[index].m_writeMutex);
    while (total < (ssize_t)msg.size()) {
        result = send(m_clients->getPollClients()[index].fd, msg.c_str() + total, msg.size() - total, MSG_NOSIGNAL);
        if (result > 0) {
            total += result;
        } else if (result == 0) {
            if ((errno == EAGAIN) || (errno == EWOULDBLOCK)) {
                return;
            }

            if (errno == EINTR) {
                continue;
            }

            m_clients->reset(index);
            return;
        } else if (result < 0) {
            m_clients->reset(index);
            return;
        }
    }
}

void Responder::sendToAll(const std::string &msg)
{
    if (msg.empty()) {
        std::cerr << "Attempting to send an empty msg" << std::endl;
        return;
    }

    for (int i = 0; i < m_clients->getMaxClients(); ++i) {
        if (m_clients->getPollClients()[i].fd != -1) {
            sendToOne(i, msg);
        }
    }
}