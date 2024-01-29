#include "chatroomclient.h"

#include <functional>

namespace grackle {

ChatroomClient::ChatroomClient(const int tcpClientIndex, const std::string &username) :
    m_tcpClientIndex(tcpClientIndex),
    m_username(username)
{

}

std::size_t ChatroomClient::operator()()
{
    return std::hash<int>{}(m_tcpClientIndex);
}

bool operator==(const ChatroomClient &lhs, const ChatroomClient &rhs)
{
    return lhs.m_tcpClientIndex == rhs.m_tcpClientIndex;
}

} /* namespace grackle */