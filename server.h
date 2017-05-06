#ifndef SIK_UDP_SERVER_H
#define SIK_UDP_SERVER_H


#include <cstddef>
#include <string>
#include <memory>

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <fcntl.h>

#include "poll.h"
#include "buffer.h"
#include "connections.h"
#include "protocol.h"
#include "communication.h"
#include "file.h"

namespace sik {
    /**
     * Exception thrown when server error occurs.
     */
    class ServerException : public Exception {
    public:
        explicit ServerException(const std::string &message) : Exception(
                message) {}

        explicit ServerException(std::string &&message) : Exception(
                std::move(message)) {}
    };

    /**
     * Server
     * @tparam buffer_size size of the datagram buffer.
     */
    template<std::size_t buffer_size>
    class Server {
        /// Data type in buffer: (arrival_time, message)
        using BufferData = std::pair<std::time_t, std::unique_ptr<Message>>;
    private:
        /// Indicates whether server should terminate.
        bool stopping = false;
        /// UDP Server socket.
        int sock;
        /// File content to add to every message.
        std::string file_content;
        /// Server address bound to the socket.
        sockaddr_in address;
        /// Poll set.
        std::unique_ptr<Poll<1>> poll;

        /// Buffer for queued Messages
        std::unique_ptr<Buffer<BufferData, buffer_size>> buffer;
        /// First message received (front of messages stack), ready to send
        std::unique_ptr<Message> current_message;

        /// Client connections
        std::unique_ptr<Connections> connections;
        /// Clients to receive current message
        std::queue<sockaddr_in> current_clients;

        /// Message sender
        std::unique_ptr<Sender> sender;
        /// Message receiver
        std::unique_ptr<Receiver> receiver;

        /**
         * Opens new UDP socket and saves it to the sock.
         * @throws ServerException when opening socket fails.
         */
        void open_socket() {
            sock = socket(AF_INET, SOCK_DGRAM | O_NONBLOCK, IPPROTO_UDP);
            if (sock < 0) {
                throw ServerException("Error opening socket");
            }
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
            int binding_result = bind(sock, (sockaddr *) &address,
                                      (socklen_t) sizeof(address));
            if (binding_result < 0) {
                throw ServerException("Error binding to socket");
            }
        }

        /**
         * Opens file socket for read only use.
         * @param filename file to open.
         */
        void read_file(const std::string &filename) {
            try {
                File file(filename);
                file_content = file.read_content(
                        PACKET_SIZE - Message::message_offset);
            } catch (const std::invalid_argument &e) {
                throw ServerException(e.what());
            } catch (const std::runtime_error &e) {
                throw ServerException(e.what());
            }
        }

        /**
         * Handles receiving data from client.
         */
        void receive() noexcept {
            sockaddr_in client_address = sockaddr_in();
            try {
                std::unique_ptr<Message> message =
                        receiver->receive_message(client_address);
                if (message->has_message()) {
                    throw std::invalid_argument(
                            "Only timestamp and a single character expected");
                }
                buffer->push(std::make_pair(std::time(0), std::move(message)));
            } catch (const std::invalid_argument &e) {
                print_error(client_address, e.what());
            } catch (const ConnectionException &) {
                // TODO: Error handling
            }

            // Add client address to send him messages.
            connections->add_client(client_address);
        }

        /**
         * Prepares data to send to client.
         */
        void prepare_send_data() {
            while (current_clients.size() == 0 && buffer->size() > 0) {
                BufferData current_item = buffer->pop();
                current_clients = connections->get_clients(current_item.first);
                current_message = std::move(current_item.second);
                current_message->set_message(file_content);
            }
        }

        /**
         * Sends data of current_message to the first client in current_clients
         * list. If there are no clients left moves onto next message.
         */
        void send() noexcept {
            prepare_send_data();
            if (current_clients.size() == 0) {
                return;
            }

            sockaddr_in client_address = current_clients.front();
            current_clients.pop();

            try {
                sender->send_message(client_address, current_message);
            } catch (const WouldBlockException &) {
                current_clients.push(client_address);
            } catch (const ConnectionException &) {
                // TODO: error handling
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
            read_file(filename);

            buffer = std::make_unique<Buffer<BufferData, buffer_size>>();
            connections = std::make_unique<Connections>();
            poll = std::make_unique<Poll<1>>();
            poll->add_descriptor(sock, POLLIN | POLLOUT);

            sender = std::make_unique<Sender>(sock);
            receiver = std::make_unique<Receiver>(sock);
        }

        /**
         * Destroys server.
         */
        ~Server() {
            if (close(sock) < 0) {
                std::cerr << "Error closing server socket" << std::endl;
            }
        }

        /**
         * Starts server loop, going forever until stop method is called
         * through system interrupt function.
         */
        void run() noexcept {
            stopping = false;
            while (!stopping) {
                poll->wait(-1);
                if (stopping) {
                    return;
                }

                if ((*poll)[sock].revents & POLLIN) {
                    receive();
                } else if ((*poll)[sock].revents & POLLOUT) {
                    send();
                }
            }
        }

        /**
         * Stops server main loop.
         */
        void stop() noexcept {
            stopping = true;
        }
    };
}

#endif //SIK_UDP_SERVER_H
