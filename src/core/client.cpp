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
    m_docParsed      = other.m_docParsed;
    m_jsonStart      = other.m_jsonStart;
}

void Client::reset()
{
    std::memset(m_header, 0, sizeof(m_header));
    std::memset(m_body, 0, sizeof(m_body));
    m_headerDone    = false;
    m_bodyLength    = 0;
    m_headerBytesRd = 0;
    m_bodyBytesRd   = 0;
    m_doc.Clear();
    m_docParsed     = false;
    m_jsonStart     = 0;
}

void Client::parseBody()
{
    int i = 0;
    for (i = 0; i < m_BODYSIZE; ++i) {
        if (m_body[i] == '{') {
            m_jsonStart = i;
            break;
        }
    }

    m_doc.Parse(&(m_body[m_jsonStart]));
    if (m_doc.HasParseError()) {
        return;
    }

    m_docParsed = true;
}

std::string Client::getBody()
{
    if (!m_docParsed) {
        return "";
    }

    rapidjson::StringBuffer buffer;
    rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
    m_doc.Accept(writer);
    return buffer.GetString();
}

std::string Client::getPath()
{
    parseBody();
    return m_doc[jsonkeys::PATH.c_str()].GetString();
}