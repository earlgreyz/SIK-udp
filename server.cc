#include <iostream>
#include "error.h"
#include "parse.h"


// Name of an executable program was run as.
std::string executable;
// Port given as first parameter.
int port;
// File which content will be added to every packet.
std::string filename;

/**
 * Prints usage.
 */
void usage() {
    std::cout << "Usage: " << executable << " port filename\n\n"
        "Parameters:\n"
        " - port        Port number, on which server listens for data\n"
        " - filename    File which content is added to udp packets\n";
}

/**
 * Parses command line arguments and saves to appropriate variables.
 * @param argc arguments count.
 * @param argv argument values.
 */
void parse_arguments(const int argc, char * const argv[]) {
    // Save executable for `usage` function.
    executable = std::move(argv[0]);

    if (argc != 3) {
        fatal("Invalid arguments count", Status::ERROR_ARGS);
    }

    try {
        port = parse_port(argv[1]);
        filename = std::move(argv[2]);
    } catch (const std::exception & e) {
        fatal(e.what(), Status::ERROR_ARGS);
    }
}

int main(int argc, char * argv[]) {
    parse_arguments(argc, argv);
    return (int)Status::OK;
}