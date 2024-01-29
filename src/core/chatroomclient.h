#ifndef _CHATROOM_CLIENT_H_
#define _CHATROOM_CLIENT_H_

#include <string>
#include <sys/socket.h>

namespace grackle {

class ChatroomClient {

private:
    int m_tcpClientIndex;
    std::string m_username;
    struct sockaddr udpAddr; // UDP is connectionless so only the address is what differentiates

public:
    ChatroomClient(const int tcpClientIndex, const std::string &username);

    ChatroomClient(const ChatroomClient &) = default;
    ChatroomClient(ChatroomClient &&) = delete;
    ChatroomClient &operator=(ChatroomClient &&) = delete;
    ChatroomClient &operator=(const ChatroomClient &) = delete;

    ~ChatroomClient() = default;

    std::size_t operator()();
    friend bool operator==(const ChatroomClient &lhs, const ChatroomClient &rhs);
}; /* ChatroomClient */

bool operator==(const ChatroomClient &lhs, const ChatroomClient &rhs);

} /* namespace grackle */

#endif /* _CHATROOM_CLIENT_H_ */