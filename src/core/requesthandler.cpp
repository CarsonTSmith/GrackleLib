#include "requesthandler.h"

#include <cstdlib>

using namespace grackle;

unsigned int RequestHandler::headerToInt(const char *header)
{
	auto ret = std::atoi(header);
	if (ret > 0) {
		return ret;
    } else {
	    return 0;
    }
}

int RequestHandler::readHeader(const int index)
{
    auto &clients = clients::clients_s::get_instance();
    const int bytesrd = read(clients.p_clients[index].fd,
                             clients.c_clients[index].header + clients.c_clients[index].header_bytes_rd,
                             client::HEADER_SIZE - clients.c_clients[index].header_bytes_rd);
    if (bytesrd < 0) {
        if ((errno == EAGAIN) || (errno == EINTR)) {
            return HEADER_NOT_DONE;
        }

        return HEADER_READ_ERROR;
    }

    if (bytesrd == 0) {
        return CLIENT_CLOSED_CONN;
    }

    clients.c_clients[index].header_bytes_rd += bytesrd;
    if (clients.c_clients[index].header_bytes_rd == client::HEADER_SIZE) {
        clients.c_clients[index].header_done = true;
        clients.c_clients[index].body_length = convert_header_to_num(clients.c_clients[index].header);
        if (clients.c_clients[index].body_length > client::BODY_SIZE) {
            return HEADER_READ_ERROR; // stop the buffer from overflowing
        }

        return HEADER_DONE;
    }

    return HEADER_NOT_DONE;
}

static int read_body(const int index)
{
    auto &clients = clients::clients_s::get_instance();
    const auto bytesrd = read(clients.p_clients[index].fd,
                              clients.c_clients[index].body + clients.c_clients[index].body_bytes_rd,
                              clients.c_clients[index].body_length - clients.c_clients[index].body_bytes_rd);
    if (bytesrd < 0) {
        if ((errno == EAGAIN) || (errno == EINTR)) {
            return BODY_NOT_DONE; 
        }

        return BODY_READ_ERROR;
    }

    if (bytesrd == 0) {
        return CLIENT_CLOSED_CONN;
    }

    clients.c_clients[index].body_bytes_rd += bytesrd;
    if (clients.c_clients[index].body_bytes_rd < clients.c_clients[index].body_length) {
        return BODY_NOT_DONE;
    }
    
    return BODY_DONE;
}

static void do_read_body(const int index)
{
    auto &clients = clients::clients_s::get_instance();
    int status = read_body(index);
    switch (status) {
    case BODY_DONE:
        // perform the request
        // then reset the client's buffers
        request_router::route(index);
        clients.c_clients[index].reset();
        break;
    case BODY_NOT_DONE:
        break; // go back to polling
    case BODY_READ_ERROR:
    case CLIENT_CLOSED_CONN:
    default:
        clients::reset(index);
        break;
    }
}

static void do_read_header(const int index)
{
    int status = read_header(index);
    switch (status) {
    case HEADER_DONE:
        do_read_body(index);
        break;
    case HEADER_NOT_DONE:
        break; // go back to polling
    case HEADER_READ_ERROR:
    case CLIENT_CLOSED_CONN:
    default:
        clients::reset(index);
        break;
    }
}

void request::handle_request(int id, const int index)
{
    auto &clients = clients::clients_s::get_instance();
    if (!clients.c_clients[index].read_mutex.try_lock()) {
        return;
    }

    if (clients.p_clients[index].fd == -1) {
        clients.c_clients[index].read_mutex.unlock();
        return;
    }

    if (clients.c_clients[index].header_done == true) {
        do_read_body(index);
    } else {
        do_read_header(index);
    }

    clients.c_clients[index].read_mutex.unlock();
}