#ifndef _CLIENT_H_
#define _CLIENT_H_

#include <mutex>
#include <rapidjson/document.h>
#include <string>
#include <utility>

namespace grackle {

class Client {

public:
    static constexpr int m_HEADERSIZE = 8;
    static constexpr int m_BODYSIZE   = 8192;

    char                m_header[m_HEADERSIZE] = { 0 };
    char                m_body[m_BODYSIZE]     = { 0 };
    bool                m_headerDone           = false;
    unsigned int        m_bodyLength           = 0;
    unsigned int        m_headerBytesRd        = 0;
    unsigned int        m_bodyBytesRd          = 0;
    std::mutex          m_readMutex;
    std::mutex          m_writeMutex;

    Client() = default;

    Client(const Client &);
    Client(Client &&) = delete;
    Client &operator=(const Client &) = delete;
    Client &operator=(Client &&) = delete;

    ~Client() = default;

    void reset();
    std::pair<std::string, std::string> parse();

private:
    std::string getBody(rapidjson::Document &doc);
    void parseBody(rapidjson::Document &doc);

}; /* class Client */

} /* namespace grackle */

#endif /* _CLIENT_H_ */