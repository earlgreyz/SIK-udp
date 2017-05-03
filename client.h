#ifndef SIK_UDP_CLIENT_H
#define SIK_UDP_CLIENT_H


#include <cstdint>
#include <cstring>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <unistd.h>
#include "error.h"
#include "protocol.h"

namespace sik {

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
        char buffer[PACKET_SIZE];
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

        /**
         * Prepares address structure.
         * @param host server host.
         * @param port server port.
         */
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

    public:
        Client(const std::string &host, uint16_t port) {
            setup_address(host, port);
            open_socket();
        }

        ~Client() {
            close(sock);
        }

        /**
         * Sends given message to server.
         * @param message message to send.
         */
        void send(const Message &message) {
            std::string bytes = message.to_bytes();
            const char * data = bytes.c_str();

            socklen_t address_len = sizeof(address);
            ssize_t sent_length = sendto(sock, data, (ssize_t)bytes.length(), 0,
                                         (sockaddr *)&address, address_len);

            if (sent_length != (ssize_t)bytes.length()) {
                // TODO: handle error
            }
        }

        /**
         * Receives data from server and prints it to the stdout. If given
         * data is not a valid message prints warning to stderr and.
         */
        void receive() {
            ssize_t length = sizeof(buffer) - 1;
            sockaddr_in server_address;
            socklen_t address_len = sizeof(server_address);

            ssize_t received_length = recvfrom(sock, buffer, length, 0,
                                               (sockaddr *)&server_address, &address_len);
            if (received_length < 0) {
                // TODO: handle error
            }

            try {
                Message message(buffer);
                message.print(std::cout);
            } catch (const std::invalid_argument& e) {
                std::cerr << "Invalid message received from "
                          << inet_ntoa(server_address.sin_addr) << ":"
                          << server_address.sin_port << " with error "
                          << e.what() << std::endl;
            }
        }

        /**
         * Starts data receiving loop.
         */
        void run() {
            while(!stopping) {
                receive();
            }
        }

        /**
         * Stops data receiving loop.
         */
        void stop() {
            stopping = true;
        };
    };
}

#endif //SIK_UDP_CLIENT_H
