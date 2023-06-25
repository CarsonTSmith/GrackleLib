#include "client.h"

#include <cstring>

using namespace grackle;

Client::Client(const Client &other)
{
    std::strcpy(m_header, other.m_header);
    std::strcpy(m_body, other.m_body);
    m_headerDone    = other.m_headerDone;
    m_bodyLength    = other.m_bodyLength;
    m_headerBytesRd = other.m_headerBytesRd;
    m_bodyBytesRd   = other.m_bodyBytesRd;
}

void Client::reset()
{
    std::memset(m_header, 0, sizeof(m_header));
    std::memset(m_body, 0, sizeof(m_body));
    m_headerDone    = false;
    m_bodyLength    = 0;
    m_headerBytesRd = 0;
    m_bodyBytesRd   = 0;
}