#ifndef SIK_UDP_CLIENT_H
#define SIK_UDP_CLIENT_H


#include <cstdint>
#include <cstring>

#define _POSIX_C_SOURCE 1

#include <sys/socket.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <unistd.h>

#include "error.h"
#include "protocol.h"
#include "communication.h"

namespace sik {

    /**
     * Exception thrown when server error occurs.
     */
    class ClientException : public Exception {
    public:
        explicit ClientException(const std::string &message) : Exception(
                message) {}

        explicit ClientException(std::string &&message) : Exception(message) {}
    };

    class Client {
    private:
        int sock;
        sockaddr_in address;
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

            if (getaddrinfo(host.c_str(), nullptr, &addr_hints, &addr_result)
                    != 0) {
                throw ClientException("Error getting address");
            }

            address.sin_family = AF_INET;
            sockaddr_in *sockaddr_result = (sockaddr_in *) addr_result->ai_addr;
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
        void send(const std::unique_ptr<Message> &message) {
            try {
                Sender(sock).send_message(address, message);
            } catch (const ConnectionException&) {
                // TODO: error handling
            }
        }

        /**
         * Receives data from server and prints it to the stdout. If given
         * data is not a valid message prints warning to stderr and.
         */
        void receive() {
            sockaddr_in server_address = sockaddr_in();
            try {
                Receiver receiver(sock);
                std::unique_ptr<Message> message
                        = receiver.receive_message(server_address);
                std::cout << *message;
                std::cout.flush();
            } catch (const std::invalid_argument& e) {
                print_message_error(address, e.what());
            } catch (const ConnectionException&) {
                // TODO: handle exception
            }
        }

        /**
         * Starts data receiving loop.
         */
        void run() {
            while (!stopping) {
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
