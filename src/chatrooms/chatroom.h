#ifndef _CHATROOM_H_
#define _CHATROOM_H_

#include "chatroomclient.h"

#include <functional>
#include <mutex>
#include <string>
#include <unordered_set>

namespace grackle {

class Chatroom {

private:
    struct ChatroomHash {
        std::size_t operator()(const Chatroom &chatroom);
    }; /* struct ChatroomHash*/


    std::string m_chatroomName;
    std::mutex m_chatroomMutex;
    std::unordered_set<ChatroomClient, ChatroomHash> m_ChatroomClients;

public:
    Chatroom(const std::string &chatroomName);

    Chatroom(const Chatroom &) = default;
    Chatroom(Chatroom &&) = delete;
    Chatroom &operator=(Chatroom &&) = delete;
    Chatroom &operator=(const Chatroom &) = delete;

    ~Chatroom() = default;

    bool addChatroomClient(ChatroomClient &&chatroomClient);
    bool removeChatroomClient();
    const std::string &getChatroomName() const;

    friend bool operator==(const Chatroom &lhs, const Chatroom &rhs);

}; /* class Chatroom */

bool operator==(const Chatroom &lhs, const Chatroom &rhs);

} /* namespace grackle */

#endif /* _CHATROOM_H_ */