#ifndef SIK_UDP_SERVER_H
#define SIK_UDP_SERVER_H

// Required in c++11 to include sys/ headers
#define _POSIX_C_SOURCE 1

#include <cstddef>
#include <string>

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <map>

#include "memory.h"
#include "error.h"

#include "poll.h"
#include "protocol.h"
#include "buffer.h"
#include "connections.h"

namespace sik {
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
        /// Indicates whether server should terminate.
        bool stopping = false;
        /// UDP Server socket.
        int sock;
        /// Server address bound to the socket.
        sockaddr_in address;
        /// Poll set.
        std::unique_ptr<Poll<MAX_CLIENTS>> poll;

        /// Buffer for data received from clients.
        char data_buffer[PACKET_SIZE];

        /// Data type in buffer: (arrival_time, message)
        using BufferData = std::pair<std::time_t, std::unique_ptr<Message>>;
        /// Buffer for queued Messages
        Buffer<BufferData, BUFFER_SIZE> buffer;

        /// Clients connection time
        Connections connections;
        std::queue<sockaddr_in> current_clients;
        std::unique_ptr<Message> current_message;

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
            sockaddr_in client_address;
            socklen_t address_len = sizeof(client_address);
            ssize_t length = recvfrom(sock, data_buffer, sizeof(data_buffer), 0,
                                      (sockaddr *) &client_address, &address_len);

            if (length < 0) {
                throw ServerException("Error on datagram to server");
            }

            try {
                buffer.push(std::make_pair(std::time(0), make_unique<Message>(data_buffer)));
                connections.add_client(client_address);
            } catch (const std::invalid_argument& e) {
                print_message_error(client_address, e.what());
            }
        }

        void send_data() {
            while (current_clients.size() == 0 && buffer.size() > 0) {
                BufferData current_item = buffer.pop();
                current_clients = connections.get_clients(current_item.first);
                current_message = std::move(current_item.second);
                current_message->set_message("Lorem ipsum");
                current_message->print(std::cout);
            }

            if (current_clients.size() == 0) {
                // Nothing to send
                return;
            }

            sockaddr_in client_address = current_clients.front();
            socklen_t address_len = sizeof(client_address);
            current_clients.pop();

            std::string bytes = current_message->to_bytes();
            const char * data = bytes.c_str();

            ssize_t sent_length = sendto(sock, data, (ssize_t)bytes.length(), 0,
                                         (sockaddr *)&client_address, address_len);

            if (sent_length != (ssize_t)bytes.length()) {
                // TODO: handle error
            }
        }

    public:
        /**
         * Creates new server instance.
         * @param port port to bind server to.
         * @param filename filename which content to add to every message sent.
         */
        Server(uint16_t port, const std::string &filename) {
            open_socket();
            bind_socket(port);

            poll = make_unique<Poll<MAX_CLIENTS>>(1);
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
