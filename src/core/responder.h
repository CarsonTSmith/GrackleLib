#ifndef _RESPONDER_H_
#define _RESPONDER_H_

#include "clients.h"

#include <memory>

namespace grackle {

class Responder {

private:
    std::shared_ptr<Clients> m_clients;

public:
    Responder(std::shared_ptr<Clients> &clients);
    void sendToOne(const int index, const std::string &msg);
    void sendToAll(const std::string &msg);

};

}


#endif /* _RESPONDER_H_ */