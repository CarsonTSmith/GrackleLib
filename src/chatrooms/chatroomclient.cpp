#include "chatroomclient.h"

#include <functional>

namespace grackle {

ChatroomClient::ChatroomClient(const int tcpClientIndex, const std::string &username) :
    m_tcpClientIndex(tcpClientIndex),
    m_username(username)
{

}

const int ChatroomClient::getTcpClientIndex() const
{
    return m_tcpClientIndex;
}

bool operator==(const ChatroomClient &lhs, const ChatroomClient &rhs)
{
    return lhs.m_tcpClientIndex == rhs.m_tcpClientIndex;
}

std::size_t ChatroomClient::ChatroomClientHash::operator()(const ChatroomClient &chatroomClient)
{
    return std::hash<int>{}(chatroomClient.getTcpClientIndex());
}

} /* namespace grackle */