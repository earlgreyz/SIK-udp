#ifndef SIK_UDP_SERVER_H
#define SIK_UDP_SERVER_H


#include <cstddef>
#include <string>
#include <memory>
#include <sys/socket.h>
#include <netinet/in.h>

#include "poll.h"
#include "error.h"
#include "protocol.h"
#include "buffer.h"

namespace sik {
    static const std::size_t MESSAGE_SIZE = sizeof(Message);
    static const std::size_t BUFFER_SIZE = 4096u;
    static const std::size_t MAX_CLIENTS = 42;

    /**
     * Exception thrown when server error occurs.
     */
    class ServerException: public Exception {
    public:
        explicit ServerException(const std::string &message): Exception(message) {}
        explicit ServerException(std::string &&message): Exception(message) {}
    };

    /**
     * UDP Server
     */
    class Server {
    private:
        /// UDP Server socket.
        int sock;
        /// Server address bound to the socket.
        sockaddr_in address;
        /// Poll set.
        std::unique_ptr<Poll<MAX_CLIENTS>> poll;
        /// Indicates whether server should terminate.
        bool stopping;
        /// Buffer for data received from clients.
        char data_buffer[MESSAGE_SIZE];
        /// Buffer for queued Messages
        Buffer<Message, BUFFER_SIZE> buffer;

        /**
         * Opens new UDP socket and saves it to the sock.
         * @throws ServerException when opening socket fails.
         */
        void open_socket() {
            if ((sock = socket(AF_INET, SOCK_DGRAM | SOCK_NONBLOCK, IPPROTO_UDP)) != -1) {
                return;
            }
            throw ServerException("Error opening socket");
        }

        /**
         * Binds socket to the given port.
         * @param port port to bind socket to.
         * @throws ServerException when binding fails.
         */
        void bind_socket(uint16_t port) {
            address.sin_family = AF_INET;
            address.sin_addr.s_addr = htonl(INADDR_ANY);
            address.sin_port = htons(port);
            if (bind(sock, (sockaddr *) &address, (socklen_t)sizeof(address)) != -1) {
                return;
            }
            throw ServerException("Error binding to socket");
        }

        /**
         * One turn of server main loop.
         */
        void cycle() {
            poll->wait(-1);
            if (stopping) {
                return;
            }

            if ((*poll)[sock].revents & POLLIN) {
                receive_data();
            } else if ((*poll)[sock].revents & POLLOUT) {
                send_data();
            }
        }

        void receive_data() {
            sockaddr_in snd_address;
            socklen_t snda_len;
            ssize_t length;

            do {
                snda_len = sizeof(snd_address);
                length = recvfrom(sock, data_buffer, sizeof(data_buffer), 0,
                                  (sockaddr *) &snd_address, &snda_len);

                if (length < 0) {
                    if (errno == EWOULDBLOCK) {
                        break;
                    }
                    throw ServerException("Error on datagram to server");
                }

                std::cout << "Read from socket " << length << " bytes: " << data_buffer << std::endl;
            } while (length > 0);
        }

        void send_data() {

        }

    public:
        /**
         * Creates new server instance.
         * @param port port to bind server to.
         * @param filename filename which content to add to every message sent.
         */
        Server(uint16_t port, const std::string &filename)
                : stopping(false) {
            open_socket();
            bind_socket(port);

            poll = std::make_unique<Poll<42>>(1);
            poll->add_descriptor(sock, POLLIN | POLLOUT);
        }

        ~Server() {
            close(sock);
        }

        void run() {
            while (!stopping) {
                cycle();
            }
        }

        void stop() {
            stopping = false;
        }
    };
}

#endif //SIK_UDP_SERVER_H
