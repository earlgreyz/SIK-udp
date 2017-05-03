#ifndef SIK_UDP_PROTOCOL_H
#define SIK_UDP_PROTOCOL_H


#include <cstdint>
#include <endian.h>
#include <cstring>
#include <stdexcept>

#include "error.h"

namespace sik {
    using timestamp_t = uint64_t;

    /// Minimum valid timestamp: Friday, 01-Jan-17 00:00:00 UTC in RFC 2822
    //const int64_t MIN_TIMESTAMP = -7983878400;
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
        /// Message character
        char character;
        /// Message content
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
        /**
         * Constructs new Message with given parameters.
         * @param timestamp message timestamp.
         * @param character message character.
         * @param message message content.
         */
        Message(timestamp_t timestamp, char character, const std::string &message)
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
                : timestamp(timestamp), character(character), message(std::move(message)) {
            validate();
        }

        /**
         * Constructs new Message from raw bytes formatted as follows:
         * - 8 bytes as timestamp (timestamp_t in big endian)
         * - 1 byte as character (char)
         * - up to 65527 bytes as message (std::string)
         * @param bytes raw bytes with given format
         */
        Message(const std::string &bytes) {
            const char *data = bytes.c_str();
            // Get first sizeof(timestamp_t) bytes and save it as timestamp
            // after converting from big endian to host number system
            timestamp = be64toh(((timestamp_t *)data)[0]);
            // Save next byte as the message character
            character = data[sizeof(timestamp_t)];
            // Save remaining bytes as message content
            message = std::move(bytes.substr(sizeof(timestamp_t) + sizeof(char) + 1));
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
            std::string bytes(sizeof(timestamp_t) + sizeof(char), 0);
            // Save timestamp in big endian form at the beginning of the data
            ((timestamp_t *)bytes.c_str())[0] = htobe64(timestamp);
            // Save character after the timestamp
            bytes[sizeof(timestamp_t)] = character;
            // Append remaining message content
            bytes.append(message);
            return std::move(bytes);
        }

        void print(std::ostream &ostream) {
            ostream << character << message;
        }
    };
}

#endif //SIK_UDP_PROTOCOL_H
