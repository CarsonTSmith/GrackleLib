#ifndef _RESPONDER_H_
#define _RESPONDER_H_

#include <memory>

namespace grackle {

class Clients;

class Responder {

private:
    std::shared_ptr<Clients> m_clients;

public:
    Responder(const std::shared_ptr<Clients> &clients);

    Responder(const Responder &) = delete;
    Responder(Responder &&) = delete;
    Responder &operator=(const Responder &) = delete;
    Responder &operator=(Responder &&) = delete;

    ~Responder() = default;

    void sendToOne(const int index, const std::string &msg);
    void sendToAll(const std::string &msg);

};

}


#endif /* _RESPONDER_H_ */