#ifndef SIK_UDP_SENDER_H
#define SIK_UDP_SENDER_H

#include <string>
#include <bits/unique_ptr.h>
#include "error.h"
#include "protocol.h"
#include "memory.h"

namespace sik {
    class ConnectionException : public Exception {
    public:
        explicit ConnectionException(const std::string &message) : Exception(
                message) {}
        explicit ConnectionException(std::string &&message) : Exception(
                std::move(message)) {}
    };

    class Sender {
    private:
        int sock;
    public:
        Sender(int sock): sock(sock) {}

        void send_message(const sockaddr_in &address, const Message &message)
                const {
            socklen_t address_len = sizeof(address);

            std::string bytes = message.to_bytes();
            const char *data = bytes.c_str();

            ssize_t length;
            length = sendto(sock, data, (ssize_t) bytes.length(), 0,
                                 (sockaddr *) &address, address_len);

            if (length != (ssize_t) bytes.length()) {
                throw ConnectionException("Error sending message");
            }
        }
    };

    class Receiver {
    private:
        int sock;
        char buffer[PACKET_SIZE];
    public:
        Receiver(int sock): sock(sock) {}

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
