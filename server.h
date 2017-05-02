#ifndef SIK_UDP_SERVER_H
#define SIK_UDP_SERVER_H


#include <cstddef>
#include <string>
#include <memory>
#include <sys/socket.h>
#include <netinet/in.h>

#include "poll.h"
#include "error.h"

const std::size_t BUFFER_SIZE = 1024u;

namespace sik {
    /**
     * Exception thrown when server error occurs.
     */
    class ServerException: public Exception {
    public:
        explicit ServerException(const std::string &message): Exception(message) {}
        explicit ServerException(std::string &&message): Exception(message) {}
    };

    /**
     *
     * UDP Server
     */
    class Server {
    private:
        int sock;
        sockaddr_in address;
        std::unique_ptr<Poll> poll;
        bool stopping;
        char buffer[BUFFER_SIZE];

        void open_socket() {
            if ((sock = socket(AF_INET, SOCK_DGRAM | SOCK_NONBLOCK, IPPROTO_UDP)) != -1) {
                return;
            }
            throw ServerException("Error opening socket");
        }

        void bind_socket(uint16_t port) {
            address.sin_family = AF_INET;
            address.sin_addr.s_addr = htonl(INADDR_ANY);
            address.sin_port = htons(port);
            if (bind(sock, (sockaddr *) &address, (socklen_t)sizeof(address)) != -1) {
                return;
            }
            throw ServerException("Error binding to socket");
        }

    public:
        Server(uint16_t port, const std::string &filename, int size)
                : stopping(false) {
            open_socket();
            bind_socket(port);
            poll = std::make_unique<Poll>(1);
            poll->add_descriptor(sock);
        }

        ~Server() {
            close(sock);
        }

        void run() {
            do {
                if (stopping) {
                    if (close(sock) == -1) {
                        throw ServerException("Error closing the sock");
                    }
                    poll->remove_descriptor(sock);
                }
                poll->wait(-1);

                for (const pollfd& client: *poll) {
                    if (client.fd == sock) {
                        handle_server(client);
                    } else {
                        handle_client(client);
                    }
                }
            } while (!stopping);
        }

        void stop() {
            stopping = false;
        }

        void handle_server(const pollfd& client) {
            if (!stopping && client.revents & POLLIN) {
                sockaddr_in client_address;
                socklen_t client_address_length;
                ssize_t length;

                do {
                    client_address_length = sizeof(client_address);
                    length = recvfrom(sock, buffer, sizeof(buffer), 0,
                                      (sockaddr *) &client_address, &client_address_length);

                    if (length < 0) {
                        if (errno == EWOULDBLOCK) {
                            break;
                        }
                        throw ServerException("Error on datagram to server");
                    }

                    std::cout << "Read from socket " << length << " bytes: " << buffer << std::endl;
                } while (length > 0);
            }
        }

        void handle_client(const pollfd& client) {

        }
    };
}

#endif //SIK_UDP_SERVER_H
