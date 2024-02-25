#include "chatroom.h"

#include <functional> // hash

namespace grackle {

Chatroom::Chatroom(const std::string &chatroomName) : m_chatroomName(chatroomName)
{
    
}

bool operator==(const Chatroom &lhs, const Chatroom &rhs)
{
    return lhs.m_chatroomName == rhs.m_chatroomName;
}

const std::string &Chatroom::getChatroomName() const
{
    return m_chatroomName;
}


} /* namespace grackle */