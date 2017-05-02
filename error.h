#ifndef SIK_UDP_ERROR_H
#define SIK_UDP_ERROR_H


#include <iostream>
#include <exception>
#include <string>

/**
 * Base class for Exception with message.
 */
class Exception: public std::exception {
protected:
    /// Error message
    const std::string message;
public:
    /**
     * Constructs Exception with given message.
     * @param message error message.
     */
    explicit Exception(const std::string &message)
            : message(message) {}

    /**
     * Constructs Exception with given message.
     * @param message error message.
     */
    explicit Exception(std::string &&message)
            : message(std::move(message)) {}

    virtual const char * what() const noexcept override {
        return this->message.c_str();
    }
};

/**
 * Available exit codes.
 */
enum class Status {
    OK = 0,
    ERROR_ARGS = 1,
};

/**
 * Prints error message, calls `usage` and exits with given error code.
 * `usage` function should be defined in main program file.
 * @param error_message error to print.
 * @param error_code program exit code.
 */
void fatal(const std::string &error_message, Status error_code) {
    std::cerr << error_message << std::endl;
    exit((int) error_code);
}

#endif //SIK_UDP_ERROR_H
