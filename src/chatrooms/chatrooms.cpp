#include "chatrooms.h"

#include "chatroom.h"
#include "chatroomclient.h"

namespace grackle {

Chatrooms &Chatrooms::getInstance()
{
    static Chatrooms c;
    return c;
}

bool Chatrooms::addChatroom(Chatroom &&chatroom)
{
    std::lock_guard<std::mutex> lk(m_chatroomsMutex);
    auto status = m_chatrooms.emplace(std::forward<Chatroom>(chatroom));
    return status.second;
}

void Chatrooms::removeChatroom(const std::string &chatroomName)
{
    std::lock_guard<std::mutex> lk(m_chatroomsMutex);
    m_chatrooms.erase(Chatroom(chatroomName));
}

bool Chatrooms::addUserToChatroom(ChatroomClient &&chatroomClient, const std::string &chatroomName)
{
    auto chatroom = m_chatrooms.find(Chatroom(chatroomName));
    if (chatroom == m_chatrooms.end()) {
        return false; // Chatroom does not exist
    } else {
        auto status = chatroom->addChatroomClient(std::move(chatroomClient));
        if (status == false) {
            return false; // failed to add client to chatroom
        } else {
            return true;
        }
    }
}

bool Chatrooms::removeUserFromChatroom(const std::string &username, const std::string &chatroomName)
{
    auto chatroom = m_chatrooms.find(Chatroom(chatroomName));
    if (chatroom == m_chatrooms.end()) {
        return false; // Chatroom does not exist
    } else {
        chatroom->erase(ChatroomClient(username));
        return true;
    }
}

const std::unordered_set<Chatroom> &getChatrooms() const
{
    return m_chatrooms;
}

std::size_t Chatrooms::ChatroomHash::operator()(const Chatroom &chatroom)
{
    return std::hash<std::string>{}(chatroom.getChatroomName());
}

} /* namespace grackle */