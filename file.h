#ifndef SIK_UDP_FILE_READER_H
#define SIK_UDP_FILE_READER_H


#include <string>
#include <stdexcept>
#include <unistd.h>
#include <fcntl.h>
#include <iostream>

class File {
private:
    /// File descriptor
    int file;
public:
    /**
     * Opens file descriptor in read only mode.
     * @param filename file to open.
     * @throws std::invalid_argument when file cannot be opened
     */
    File(const std::string &filename) {
        if ((file = open(filename.c_str(), O_RDONLY)) == -1) {
            throw std::invalid_argument("Could not open file");
        }
    }

    /**
     * Reads file content.
     * @return file content.
     * @throws std::runtime_error if reading file returns an error
     */
    std::string read_content() const {
        char buffer[64 * 1024u];
        std::string content;

        ssize_t length;
        do {
            length = read(file, buffer, sizeof(buffer) - 1);
            if (length < 0) {
                throw std::runtime_error("Error reading file");
            }
            buffer[length] = '\0';
            content.append(buffer);
        } while (length > 0);
        return std::move(content);
    }

    /**
     * Destroys file instance. Closes the descriptor.
     */
    ~File() {
        if (close(file) == -1) {
            std::cerr << "Error closing file" << std::endl;
        }
    }
};

#endif //SIK_UDP_FILE_READER_H
