#include <iostream>
#include "error.h"


// Name of an executable program was run as.
std::string executable;
// Port given as first parameter.
int port;
// File which content will be added to every packet.
std::string filename;

/**
 * Prints usage.
 */
void usage() const {
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
    executable = argv[0];
    if (argc != 3) {
        fatal("Invalid arguments count", Status::ERROR_ARGS);
    }

    try {
        port = std::stoi(argv[1]);
    } catch (const std::exception &) {
        fatal("Port must be an integer", Status::ERROR_ARGS);
    }

    filename = argv[2];
}

int main(int argc, char * argv[]) {
    parse_arguments(argc, argv);
    return (int)Status::OK;
}