#ifndef SIK_UDP_SENDER_H
#define SIK_UDP_SENDER_H

#include <string>
#include "memory.h"
#include "error.h"
#include "protocol.h"

namespace sik {
    /**
     * Exception thrown when Sender or Receiver error occurs.
     */
    class ConnectionException : public Exception {
    public:
        explicit ConnectionException(const std::string &message) : Exception(
                message) {}
        explicit ConnectionException(std::string &&message) : Exception(
                std::move(message)) {}
    };

    /**
     * Class for sending messages over socket.
     */
    class Sender {
    private:
        /// Socket to send data to.
        int sock;
    public:
        /**
         * Creates new sender.
         * @param sock UDP Socket.
         */
        Sender(int sock) noexcept : sock(sock) {}

        /**
         * Sends message to given address.
         * @param address receiver address.
         * @param message message to send.
         * @throws ConnectionException when sendto finishes with error
         */
        void send_message(const sockaddr_in &address,
                          const std::unique_ptr<Message> &message) const {
            socklen_t address_len = sizeof(address);

            std::string bytes = message->to_bytes();
            const char *data = bytes.c_str();

            ssize_t length;
            length = sendto(sock, data, (ssize_t) bytes.length(), 0,
                            (sockaddr *) &address, address_len);

            if (length != (ssize_t) bytes.length()) {
                throw ConnectionException("Error sending message");
            }
        }
    };

    /**
     * Class for receiving messages from socket.
     */
    class Receiver {
    private:
        /// Socket to receive data from
        int sock;
        /// Buffer for received data
        char buffer[PACKET_SIZE];
    public:
        /**
         * Creates new receiver.
         * @param sock socket to send data to.
         */
        Receiver(int sock) noexcept : sock(sock) {}

        /**
         * Receives message from socket.
         * @param address sender address.
         * @return received message
         * @throws std::invalid_parameter if received message is not a valid
         * message
         * @throws ConnectionException if recvfrom finishes with error
         */
        std::unique_ptr<Message> receive_message(const sockaddr_in &address) {
            socklen_t address_len = sizeof(address);

            ssize_t length;
            length = recvfrom(sock, buffer, sizeof(buffer) - 1, 0,
                              (sockaddr *) &address, &address_len);
            if (length < 0) {
                throw ConnectionException("Error receiving data");
            }
            buffer[length + 1] = '\0';

            return make_unique<Message>(buffer, (std::size_t) length);
        }
    };
}

#endif //SIK_UDP_SENDER_H
