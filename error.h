#ifndef SIK_UDP_ERROR_H
#define SIK_UDP_ERROR_H


#include <iostream>

/**
 * Prints usage - should be defined in program main file.
 */
extern void usage() const;

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
void fatal(const std::string &error_message, Status error_code) const {
    std::cerr << error_message << std::endl;
    usage();
    exit((int)error_code);
}


#endif //SIK_UDP_ERROR_H
