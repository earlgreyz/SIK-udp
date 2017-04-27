#ifndef SIK_UDP_PARSE_H
#define SIK_UDP_PARSE_H


#include <exception>
#include <string>
#include "protocol.h"

/**
 * Exception thrown when parsing error occurs.
 */
class ParseException: public std::exception {
private:
    /// Error message
    const std::string message;
public:
    /**
     * Constructs ParseException with given message.
     * @param message error message.
     */
    explicit ParseException(const std::string &message)
            : message(message) {}

    /**
     * Constructs ParseException with given message.
     * @param message error message.
     */
    explicit ParseException(std::string &&message)
            : message(std::move(message)) {}

    virtual const char * what() const noexcept override {
        return this->message.c_str();
    }
};

/**
 * Converts string to port number.
 * @param input string to convert.
 * @return port number.
 * @throws ArgumentException if input is not a valid port number.
 */
int parse_port(const std::string &input) {
    try {
        int port = std::stoi(input);
        return port;
    } catch (const std::exception &) {
        throw new ParseException("Port must be an integer");
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
        throw new ParseException("Character must be a single character");
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
    throw new ParseException("Parsing timestamp not implemented yet");
}


#endif //SIK_UDP_PARSE_H
