#ifndef SIK_UDP_PROTOCOL_H
#define SIK_UDP_PROTOCOL_H


#include <cstdint>
#include "protocol.h"

namespace sik {
    using timestamp_t = uint64_t;

    /// Minimum valid timestamp: Friday, 01-Jan-17 00:00:00 UTC in RFC 2822
    const int64_t MIN_TIMESTAMP = -7983878400;
    /// Maximum valid timestamp: Friday, 31-Dec-41 23:59:59 UTC in RFC 2822
    const timestamp_t MAX_TIMESTAMP = 71697398399u;

    /**
     * Message structure.
     */
    struct Message {
        timestamp_t timestamp;
        char character;
        char *message;

        Message(timestamp_t timestamp = 0, char character = 0, char *message = nullptr)
                : timestamp(timestamp), character(character), message(message) {}
    };

    /**
     * Validates timestamp.
     * @param timestamp timestamp to validate.
     * @return whether timestamp is valid.
     */
    bool inline is_proper_timestamp(timestamp_t timestamp) {
        return timestamp <= MAX_TIMESTAMP;
    }

    /**
     * Validates message.
     * @param message message to validate.
     * @return whether message is valid.
     */
    bool inline is_proper_message(const Message& message) {
        return is_proper_timestamp(message.timestamp);
    }
}

#endif //SIK_UDP_PROTOCOL_H
