#ifndef SIK_UDP_PARSE_H
#define SIK_UDP_PARSE_H


#include <exception>
#include <string>
#include <boost/lexical_cast.hpp>

#include "error.h"
#include "protocol.h"

namespace sik {
    /**
     * Exception thrown when parsing error occurs.
     */
    class ParseException: public Exception {
    public:
        explicit ParseException(const std::string &message): Exception(message) {}
        explicit ParseException(std::string &&message): Exception(message) {}
    };

    /**
     * Converts string to port number.
     * @param input string to convert.
     * @return port number.
     * @throws ArgumentException if input is not a valid port number.
     */
    uint16_t parse_port(const std::string &input) {
        try {
            uint16_t port = boost::lexical_cast<uint16_t>(input);
            if (boost::lexical_cast<std::string>(port) != input || port == 0) {
                throw ParseException("Port must be an integer between 1 and 65,535");
            }
            return port;
        } catch (const boost::bad_lexical_cast &) {
            throw ParseException("Port must be an integer between 1 and 65,535");
        }
    }

    /**
     * Converts string to a single character.
     * @param input string to convert.
     * @return single character.
     * @throws ArgumentException if input is not a single character.
     */
    char parse_character(const std::string &input) {
        if (input.length() != 1) {
            throw ParseException("Character must be a single character");
        }
        return input[0];
    }

    /**
     * Converts string to timestamp.
     * @param input string to convert.
     * @return timestamp.
     * @throws ArgumentException if input is not a valid timestamp.
     */
    timestamp_t parse_timestamp(const std::string &input) {
        try {
            timestamp_t timestamp = boost::lexical_cast<timestamp_t>(input);
            if (boost::lexical_cast<std::string>(timestamp) != input
                    || !is_proper_timestamp(timestamp)) {
                throw ParseException("Timestamp must be a 64-bit unsigned number");
            }
            return timestamp;
        } catch (const boost::bad_lexical_cast &) {
            throw ParseException("Timestamp must be a 64-bit unsigned number");
        }
    }
}

#endif //SIK_UDP_PARSE_H
