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
     * @param buffer_size number of bytes to read.
     * @return file content.
     * @throws std::runtime_error if reading file returns an error
     */
    std::string read_content(std::size_t buffer_size) const {
        char buffer[buffer_size];
        ssize_t length = read(file, buffer, buffer_size);
        if (length < 0) {
            throw std::runtime_error("Error reading file");
        } else if ((std::size_t) length == buffer_size) {
            std::cerr << "File too long, reading " << length << " bytes."
                      << std::endl;
            length--;
        }
        buffer[length] = '\0';
        std::string content = buffer;
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
