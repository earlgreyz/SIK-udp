#include <iostream>
#include <csignal>

#include "memory.h"
#include "error.h"
#include "parse.h"
#include "server.h"

const std::size_t BUFFER_SIZE = 4096u;

// Name of an executable program was run as.
std::string executable;
// Port given as first parameter.
uint16_t port;
// File which content will be added to every packet.
std::string filename;
// Server
std::unique_ptr<sik::Server<BUFFER_SIZE>> server;

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
        usage();
        fatal("Invalid arguments count", Status::ERROR_ARGS);
    }

    try {
        port = sik::parse_port(argv[1]);
        filename = std::move(argv[2]);
    } catch (const sik::ParseException &e) {
        usage();
        fatal(e.what(), Status::ERROR_ARGS);
    }
}

/**
 * Registers SIGINT signal.
 */
void register_signals() {
    if (signal(SIGINT, [](int sig) {
        server->stop();
        std::cerr << "Signal " << sig << " Stopping server." << std::endl;
    }) == SIG_ERR) {
        fatal("Unable to register SIGINT signal", Status::ERROR_ARGS);
    }
}

int main(int argc, char * argv[]) {
    parse_arguments(argc, argv);
    register_signals();

    try {
        server = make_unique<sik::Server<BUFFER_SIZE>>(port, filename);
    } catch (const sik::ServerException &e) {
        fatal(e.what(), Status::ERROR_ARGS);
    } catch (const sik::PollException &e){
        fatal(e.what(), Status::ERROR_ARGS);
    }

    server->run();
    return (int) Status::OK;
}