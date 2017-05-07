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
        /// Socket
        int sock;
        /// Server address
        sockaddr_in address;
        /// Indicates whether client loop should stop
        bool stopping = false;
        /// Message sender
        std::unique_ptr<Sender> sender;
        /// Message receiver
        std::unique_ptr<Receiver> receiver;

        /**
         * Opens new UDP socket and saves it to the sock.
         * @throws ServerException when opening socket fails.
         */
        void open_socket() {
            if ((sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) < 0) {
                throw ClientException("Error opening socket");
            }
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

            sender = std::make_unique<Sender>(sock);
            receiver = std::make_unique<Receiver>(sock);
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
                sender->send_message(address, message);
            } catch (const std::exception&) {
                std::cerr << "Error occurred while sending message to server"
                          << std::endl;
            }
        }

        /**
         * Receives data from server and prints it to the stdout. If given
         * data is not a valid message prints warning to stderr and.
         */
        void receive() {
            sockaddr_in server_address = sockaddr_in();
            try {
                std::unique_ptr<Message> message
                        = receiver->receive_message(server_address);
                std::cout << *message << std::endl;
            } catch (const std::invalid_argument& e) {
                print_error(address, e.what());
            } catch (const ConnectionException&) {
                std::cerr << "Unexpected error occurred while receiving message"
                          << std::endl;
            }
        }

        /**
         * Starts data receiving loop.
         */
        void run() {
            stopping = false;
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
