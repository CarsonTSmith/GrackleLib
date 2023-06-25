#ifndef _CLIENT_H_
#define _CLIENT_H_

#include <mutex>
#include <rapidjson/document.h>
#include <string>

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
    rapidjson::Document m_doc;
    bool                m_docParsed            = false;
    unsigned int        m_jsonStart            = 0; // index where the first { in the body is
    

    Client() = default;
    Client(const Client &);
    void reset();
    std::string getBody();
    std::string getPath();
    void parseBody();

}; /* class Client */

} /* namespace grackle */

#endif /* _CLIENT_H_ */