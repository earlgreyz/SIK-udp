#ifndef SIK_UDP_ERROR_H
#define SIK_UDP_ERROR_H


#include <iostream>
#include <exception>
#include <string>

#include <netinet/in.h>
#include <arpa/inet.h>

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
 * Prints error after receiving invalid message.
 * @param address address of client who sent an invalid message.
 * @param e error message.
 */
void inline print_error(sockaddr_in address, const std::string &e) {
    std::cerr << "Invalid message received from "
              << inet_ntoa(address.sin_addr) << ":" << address.sin_port
              << " with error " << e << std::endl;
}

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
    std::cerr << std::endl << "Process finished with error: "
              << error_message << std::endl;
    exit((int) error_code);
}

#endif //SIK_UDP_ERROR_H
