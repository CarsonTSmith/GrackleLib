#ifndef _CLIENT_H_
#define _CLIENT_H_

#include <mutex>

namespace grackle {

class Client {

private:
    static constexpr int m_HEADERSIZE = 8;
    static constexpr int m_BODYSIZE   = 8192;

    char       m_header[m_HEADERSIZE] = { 0 };
    char       m_body[m_BODYSIZE]     = { 0 };
    bool       m_headerDone           = false;
    uint32_t   m_bodyLength           = 0;
    uint32_t   m_headerBytesRd        = 0;
    uint32_t   m_bodyBytesRd          = 0;
    std::mutex m_readMutex;
    std::mutex m_writeMutex;

public:
    Client() = default;
    Client(const Client &);
    void reset();

}; /* class Client */

} /* namespace grackle */

#endif /* _CLIENT_H_ */