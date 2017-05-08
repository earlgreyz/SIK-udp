#ifndef SIK_UDP_SENDER_H
#define SIK_UDP_SENDER_H

#include <string>
#include <memory>

#include "protocol.h"

namespace sik {
    /**
     * Exception thrown when Sender or Receiver error occurs.
     */
    class ConnectionException : public std::exception {};

    /**
     * Exception thrown when function finishes with EWOULDBLOCK errno.
     */
    class WouldBlockException : public std::exception {};

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
         * @param with_message send message with content.
         * @throws WouldBlockException if sendto finishes with errno EWOULDBLOCK
         * @throws ConnectionException when sendto finishes with error
         */
        void send_message(const sockaddr_in &address,
                          const std::unique_ptr<Message> &message,
                          bool with_message = false) const {
            socklen_t address_len = sizeof(address);

            std::string bytes = message->to_bytes();
            char data[bytes.length() + 1];
            ssize_t data_length = bytes.length();

            // We have to null terminate the string before sending.
            std::memcpy(data, bytes.c_str(), bytes.length());
            if (with_message) {
                data[data_length] = '\0';
                data_length++;
            }

            ssize_t length = sendto(sock, data, data_length, 0,
                            (sockaddr *) &address, address_len);

            if (length < 0 && errno == EWOULDBLOCK) {
                throw WouldBlockException();
            }

            if (length != data_length) {
                throw ConnectionException();
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
         * @param with_message whether message should contain message.
         * @return received message
         * @throws std::invalid_parameter if received message is not a valid
         * message
         * @throws ConnectionException if recvfrom finishes with error
         */
        std::unique_ptr<Message> receive_message(const sockaddr_in &address,
                                                 bool with_message = false) {
            socklen_t address_len = sizeof(address);

            ssize_t length;
            length = recvfrom(sock, buffer, sizeof(buffer) - 1, 0,
                              (sockaddr *) &address, &address_len);
            if (length < 0) {
                throw ConnectionException();
            }

            if (with_message ) {
                if ((std::size_t) length <= Message::message_offset) {
                    throw std::invalid_argument("Message must contain text");
                } else if (buffer[length - 1] != '\0') {
                    throw std::invalid_argument(
                            "Message text must be null terminated");
                }
            }
            buffer[length] = '\0';

            return std::make_unique<Message>(buffer, (std::size_t) length);
        }
    };
}

#endif //SIK_UDP_SENDER_H
