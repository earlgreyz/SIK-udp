#ifndef SIK_UDP_PROTOCOL_H
#define SIK_UDP_PROTOCOL_H


#include <cstdint>
#include <endian.h>
#include <cstring>
#include <stdexcept>

namespace sik {
    using timestamp_t = uint64_t;

    /// Minimum valid timestamp: Friday, 01-Jan-17 00:00:00 UTC in RFC 2822
    // const int64_t MIN_TIMESTAMP = -7983878400;
    /// Maximum valid timestamp: Friday, 31-Dec-41 23:59:59 UTC in RFC 2822
    const timestamp_t MAX_TIMESTAMP = 71697398399u;
    /// Maximum IP Packet size - 64KB.
    const std::size_t PACKET_SIZE = 65536u;

    /**
     * Validates timestamp.
     * @param timestamp timestamp to validate.
     * @return whether timestamp is valid.
     */
    bool inline is_proper_timestamp(timestamp_t timestamp) {
        return timestamp <= MAX_TIMESTAMP;
    }

    /**
     * Message structure.
     */
    class Message {
    private:
        /// Message timestamp
        timestamp_t timestamp;
        /// Message character.
        char character;
        /// Message content.
        std::string message;


        /**
         * Validates if the message data is compatible with the specification.
         * @throws std::invalid_argument when message data is invalid.
         */
        inline void validate() const {
            if (!is_proper_timestamp(timestamp)) {
                throw std::invalid_argument("Invalid timestamp");
            }
        }

    public:
        // Bytes offset of message content.
        static const std::size_t message_offset
                = sizeof(timestamp_t) + sizeof(char);

        Message(const Message &) = delete;

        Message(Message &&m) : timestamp(m.timestamp), character(m.character),
                               message(std::move(m.message)) {}

        /**
         * Constructs new Message with given parameters.
         * @param timestamp message timestamp.
         * @param character message character.
         * @param message message content.
         */
        Message(timestamp_t timestamp, char character,
                const std::string &message)
                : timestamp(timestamp), character(character), message(message) {
            validate();
        }

        /**
         * Constructs new Message with given parameters.
         * @param timestamp message timestamp.
         * @param character message character.
         * @param message message content.
         */
        Message(timestamp_t timestamp, char character, std::string &&message)
                : timestamp(timestamp), character(character),
                  message(std::move(message)) {
            validate();
        }

        /**
         * Constructs new Message from raw bytes formatted as follows:
         * - 8 bytes as timestamp (timestamp_t in big endian)
         * - 1 byte as character (char)
         * - up to 65527 bytes as message (std::string)
         * @param bytes raw bytes with given format
         */
        Message(const char *bytes, std::size_t length) {
            if (length < message_offset) {
                throw std::invalid_argument("Invalid message data");
            }
            // Get first sizeof(timestamp_t) bytes and save it as timestamp
            // after converting from big endian to host number system
            timestamp = be64toh(((timestamp_t *) bytes)[0]);
            // Save next byte as the message character
            character = bytes[sizeof(timestamp_t)];
            // Save remaining bytes as message content
            const char *data = bytes + message_offset;
            message = data;
            validate();
        }

        /**
         * Converts message to raw bytes formatted as follows:
         * - 8 bytes as timestamp (timestamp_t in big endian)
         * - 1 byte as character (char)
         * - up to 65527 bytes as message (std::string)
         * @return raw bytes representing message
         */
        std::string to_bytes() const noexcept {
            std::string bytes(message_offset, 0);
            // Save timestamp in big endian form at the beginning of the data
            ((timestamp_t *) bytes.c_str())[0] = htobe64(timestamp);
            // Save character after the timestamp
            bytes[sizeof(timestamp_t)] = character;
            // Append remaining message content
            bytes.append(message);
            return std::move(bytes);
        }

        /**
         * Sets message to given string.
         * @param msg new message.
         * @throws std::invalid_argument if message is too long.
         */
        void set_message(const std::string &msg) {
            if (msg.length() > PACKET_SIZE - message_offset) {
                throw std::invalid_argument("Message to long");
            }
            message = msg;
        }

        /**
         * Sets message to given string.
         * @param msg new message.
         * @throws std::invalid_argument if message is too long.
         */
        void set_message(std::string &&msg) {
            if (msg.length() > PACKET_SIZE - message_offset) {
                throw std::invalid_argument("Message to long");
            }
            message = std::move(msg);
        }

        friend std::ostream &operator<<(std::ostream &, const Message &);
    };

    /**
     * Prints Message.
     * @param os stream to print to.
     * @param m message to print.
     * @return stream.
     */
    std::ostream &operator<<(std::ostream &os, const Message &m) {
        os << m.timestamp << " " << m.character << " " << m.message;
        return os;
    }
}

#endif //SIK_UDP_PROTOCOL_H
