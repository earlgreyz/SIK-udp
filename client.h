#ifndef SIK_UDP_CLIENT_H
#define SIK_UDP_CLIENT_H


#include <cstdint>
#include <cstring>
#include <sys/socket.h>
#include <netdb.h>
#include <unistd.h>
#include "error.h"
#include "protocol.h"

namespace sik {
    /// Maximum IP Packet size is 64 KB we don't want our message to be trimmed.
    static const std::size_t BUFFER_SIZE = 65536;

    /**
     * Exception thrown when server error occurs.
     */
    class ClientException: public Exception {
    public:
        explicit ClientException(const std::string &message): Exception(message) {}
        explicit ClientException(std::string &&message): Exception(message) {}
    };

    class Client {
    private:
        int sock;
        sockaddr_in address;
        char buffer[BUFFER_SIZE];
        bool stopping = false;

        /**
         * Opens new UDP socket and saves it to the sock.
         * @throws ServerException when opening socket fails.
         */
        void open_socket() {
            if ((sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) != -1) {
                return;
            }
            throw ClientException("Error opening socket");
        }

        void setup_address(const std::string &host, uint16_t port) {
            addrinfo addr_hints;
            addrinfo *addr_result;
            memset(&addr_hints, 0, sizeof(addr_hints));
            addr_hints.ai_family = AF_INET;
            addr_hints.ai_socktype = SOCK_DGRAM;
            addr_hints.ai_protocol = IPPROTO_UDP;

            if (getaddrinfo(host.c_str(), nullptr, &addr_hints, &addr_result) != 0) {
                throw ClientException("Error getting address");
            }

            address.sin_family = AF_INET;
            sockaddr_in * sockaddr_result = (sockaddr_in *) addr_result->ai_addr;
            address.sin_addr.s_addr = sockaddr_result->sin_addr.s_addr;
            address.sin_port = htons(port);

            freeaddrinfo(addr_result);
        }

        void receive() {

        }

    public:
        Client(const std::string &host, uint16_t port) {
            setup_address(host, port);
            open_socket();
        }

        ~Client() {
            close(sock);
        }

        void send(const Message& message) {

        }

        void run() {
            while(!stopping) {
                receive();
            }
        }

        void stop() {
                stopping = true;
        };
    };
}

#endif //SIK_UDP_CLIENT_H
