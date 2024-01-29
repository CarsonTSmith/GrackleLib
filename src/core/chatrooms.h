#ifndef _CHATROOMS_H_
#define _CHATROOMS_H_

#include "chatroom.h"

#include <mutex>
#include <string>
#include <unordered_set>

namespace grackle {

class ChatroomClient;

class Chatrooms {

private:
    std::unordered_set<Chatroom, Chatroom()> m_chatrooms;
    std::mutex m_chatroomsMutex;

    Chatrooms() = default;

public:
    Chatrooms(const Chatrooms &) = delete;
    Chatrooms(Chatrooms &&) = delete;
    Chatrooms &operator=(Chatrooms &&) = delete;
    Chatrooms &operator=(const Chatrooms &) = delete;

    ~Chatrooms() = default;

    Chatrooms &getInstance();

    bool addChatroom(Chatroom &&chatroom);
    void removeChatroom(const std::string &chatroomName);
    bool addUserToChatroom(const ChatroomClient &ChatroomClient, const std::string &chatroomName);
    bool removeUserFromChatroom();

    const std::unordered_set<Chatroom> &getChatrooms() const;

}; /* class Chatrooms */

} /* namespace grackle */

#endif /* _CHATROOMS_H_ */