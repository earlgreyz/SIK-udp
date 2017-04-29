#ifndef SIK_UDP_PROTOCOL_H
#define SIK_UDP_PROTOCOL_H


#include <cstdint>
#include <string>

namespace sik {
    using timestamp_t = uint64_t;

    /// Minimum valid timestamp: Friday, 01-Jan-17 00:00:00 UTC in RFC 2822
    const int64_t MIN_TIMESTAMP = -7983878400;
    /// Maximum valid timestamp: Friday, 31-Dec-41 23:59:59 UTC in RFC 2822
    const timestamp_t MAX_TIMESTAMP = 71697398399u;

    /**
     * Validates timestamp.
     * @param timestamp
     * @return
     */
    bool inline is_proper_timestamp(timestamp_t timestamp) {
        return timestamp <= MAX_TIMESTAMP;
    }
}

#endif //SIK_UDP_PROTOCOL_H
