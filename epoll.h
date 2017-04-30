/**
 * @file Custom wrappers for functions from `sys/epoll` to provide modern c++
 * exceptions instead of return codes.
 */
#ifndef SIK_UDP_EPOLL_H
#define SIK_UDP_EPOLL_H


#include <string>
#include <sys/epoll.h>
#include <unistd.h>

namespace sik {
    /**
     * Exception thrown when server error occurs.
     */
    class EpollError: public std::exception {
    private:
        /// Error message
        const std::string message;
    public:
        /**
         * Constructs EpollError with given message.
         * @param message error message.
         */
        explicit EpollError(const std::string &message)
                : message(message) {}

        /**
         * Constructs EpollError with given message.
         * @param message error message.
         */
        explicit EpollError(std::string &&message)
                : message(std::move(message)) {}

        virtual const char * what() const noexcept override {
            return this->message.c_str();
        }
    };

    class Epoll {
    private:
        int epfd;
        void control(int op, int fd, uint32_t EVENTS);
    public:
        /**
         * Creates new epoll instance. Opens the epoll file descriptor.
         * @param size hint to the kernel about the number of file descriptors
         * that are going to be monitored (not a maximum number)
         */
        Epoll(int size);
        /// Remove copy constructor
        Epoll(const Epoll&) = delete;
        /// Make move constructor the default one
        Epoll(Epoll&&) = default;
        /**
         * Destroys epoll instance.
         */
        ~Epoll();

        /**
         * Adds new file descriptor to the epoll.
         * @param fd file descriptor to watch.
         */
        void add(int fd);
    };

    Epoll::Epoll(int size) {
        epfd =  epoll_create(size);
        if (epfd == 0) {
            return;
        }

        switch (errno) {
            case EINVAL: throw EpollError("Invalid size");
            case ENFILE: throw EpollError("Total number of file descriptors reached");
            case ENOMEM: throw EpollError("Insufficient memory");
            default: throw EpollError("Unknown exception");
        }
    }

    Epoll::~Epoll() {
        close(epfd);
    }

    void Epoll::control(int op, int fd, uint32_t events) {
        epoll_event event;
        event.events = events;
        event.data.fd = fd;

        if (epoll_ctl(epfd, op, fd, &event) == 0) {
            return;
        }

        switch (errno) {
            case EBADF: throw EpollError("Invalid file descriptor");
            case EEXIST: throw EpollError("File descriptor already associated with epfd");
            case EINVAL: throw EpollError("Invalid arguments");
            case ENOENT: throw EpollError("File descriptor not associated with epfd");
            case ENOMEM: throw EpollError("Insufficient memory");
            case EPERM: throw EpollError("File descriptor doesn't suppor epoll");
            default: throw EpollError("Unknown exception");
        }
    }

    void Epoll::add(int fd) {
        control(EPOLL_CTL_ADD, fd, EPOLLIN | EPOLLOUT);
    }
}

#endif //SIK_UDP_EPOLL_H
