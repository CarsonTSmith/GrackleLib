#include "client.h"

#include "jsonkeys.h"

#include <cstring>
#include <rapidjson/document.h>
#include <rapidjson/stringbuffer.h>
#include <rapidjson/writer.h>

using namespace grackle;

Client::Client(const Client &other)
{
    std::strcpy(m_header, other.m_header);
    std::strcpy(m_body, other.m_body);
    m_headerDone     = other.m_headerDone;
    m_bodyLength     = other.m_bodyLength;
    m_headerBytesRd  = other.m_headerBytesRd;
    m_bodyBytesRd    = other.m_bodyBytesRd;
}

void Client::parseBody(rapidjson::Document &doc)
{
    int i = 0;
    for (i = 0; i < m_BODYSIZE; ++i) {
        if (m_body[i] == '{') {
            break;
        }
    }

    doc.Parse(&(m_body[i]));
}

std::string Client::getBody(rapidjson::Document &doc)
{
    rapidjson::StringBuffer buffer;
    rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
    doc.Accept(writer);
    return buffer.GetString();
}

std::pair<std::string, std::string> Client::parse()
{
    rapidjson::Document doc;
    parseBody(doc);
    auto body = getBody(doc);
    return std::make_pair(doc[jsonkeys::PATH.c_str()].GetString(), body);
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