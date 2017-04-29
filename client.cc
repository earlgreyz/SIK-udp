#include <cstdint>
#include <iostream>
#include "error.h"
#include "protocol.h"
#include "parse.h"


const int DEFAULT_PORT = 20160;

// Name of an executable program was run as.
std::string executable;
// Timestamp to send in packet.
sik::timestamp_t timestamp;
// Character to send in packet.
char character;
// Server host.
std::string host;
// Port given as first parameter.
int port = DEFAULT_PORT;

/**
 * Prints usage.
 */
void usage() {
    std::cout << "Usage: " << executable << " timestamp c host [port]\n\n"
            "Parameters:\n"
            " - timestamp   Timestamp sent in packet\n"
            " - c           Character sent in packet\n"
            " - host        Server to connect to\n"
            " - port        Optional server port (default: 20160)\n";
}

/**
 * Parses command line arguments and saves to appropriate variables.
 * @param argc arguments count.
 * @param argv argument values.
 */
void parse_arguments(const int argc, char * const argv[]) {
    // Save executable for `usage` function.
    executable = std::move(argv[0]);

    if (argc < 4 || argc > 5) {
        fatal("Invalid arguments count", Status::ERROR_ARGS);
    }

    try {
        timestamp = sik::parse_timestamp(argv[1]);
        character = sik::parse_character(argv[2]);
        host = argv[3];
        if (argc == 5) {
            port = sik::parse_port(argv[4]);
        }
    } catch (const sik::ParseException &e) {
        fatal(e.what(), Status::ERROR_ARGS);
    }
}

int main(int argc, char * argv[]) {
    parse_arguments(argc, argv);
    return (int)Status::OK;
}