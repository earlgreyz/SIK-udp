#ifndef SIK_UDP_POLL_H
#define SIK_UDP_POLL_H

#include <stdexcept>
#include <sys/poll.h>
#include "error.h"

namespace sik {
    static const std::size_t MAX_CLIENTS = 128;
    /**
     * Exception thrown when poll error occurs.
     */
    class PollException : public Exception {
    public:
        explicit PollException(const std::string &message) : Exception(
                message) {}
        explicit PollException(std::string &&message) : Exception(message) {}
    };

    /**
     * Exception called when poll timeouts.
     */
    class PollTimeoutException: public std::exception {};

    class Poll {
    private:
        /// Poll set.
        pollfd clients[MAX_CLIENTS];
        /// Maximum size of the poll set.
        const std::size_t clients_length;
        /// Current size of the poll set.
        std::size_t active_clients_count;

        /**
         * Resets the element of the poll set to default state.
         * @param client element of the poll set.
         */
        void reset_client(pollfd &client) noexcept {
            client.fd = -1;
            client.events = POLLIN;
            client.revents = 0;
        }

        /**
         * If fd matches the fd of an element in the clients array the function
         * returns the index of the element in the clients array.
         * @param fd file descriptor.
         * @return index corresponding to the given fd.
         * @throws std::out_of_range when fd is not in the poll set.
         */
        std::size_t get_index(int fd) const {
            for (std::size_t i = 0; i < clients_length; i++) {
                if (clients[i].fd == fd) {
                    return i;
                }
            }
            throw std::out_of_range("fd not in poll");
        }

    public:
        /**
         * Constructs new poll set.
         * @param clients_length maximum size of poll set.
         */
        Poll(std::size_t clients_length) noexcept
                : clients_length(clients_length), active_clients_count(0u) {
            for (pollfd& client: clients) {
                reset_client(client);
            }
        }

        /**
         * If fd matches the fd of an element in the clients array the function
         * returns a reference to the element.
         * @param fd file descriptor
         * @return pollfd corresponding to the given fd
         * @throws std::invalid_argument when fd is not a valid file descriptor.
         */
        const pollfd &operator[](int fd) const {
            if (fd < 0) {
                throw std::invalid_argument("fd must be a positive integer");
            }
            return clients[get_index(fd)];
        }

        /**
         * Adds descriptor to the poll set.
         * @param fd file descriptor to add.
         * @throws std::invalid_argument when fd is not a valid file descriptor.
         * @throws std::overflow_error when clients array is full.
         * @throws PollException when file descriptor is already in the poll set.
         */
        void add_descriptor(int fd) {
            if (fd < 0) {
                throw std::invalid_argument("fd must be a positive integer");
            }

            try {
                get_index(fd);
                throw PollException("fd already in the poll");
            } catch (const std::out_of_range&) { }

            try {
                std::size_t index = get_index(-1);
                clients[index].fd = fd;
                active_clients_count++;
            } catch (const std::out_of_range&) {
                throw std::overflow_error("limit exceeded");
            }
        }

        /**
         * Removes descriptor from the poll set.
         * @param fd file descriptor to remove.
         * @throws std::invalid_argument when fd is not a valid file descriptor.
         * @throws std::out_of_range when fd is not in the set.
         */
        void remove_descriptor(int fd) {
            if (fd < 0) {
                throw std::invalid_argument("fd must be a positive integer");
            }

            std::size_t index = get_index(fd);
            clients[index] = clients[active_clients_count];
            reset_client(clients[active_clients_count]);
            active_clients_count--;
        }

        /**
         * Waits for events up to a timeout.
         * @param timeout milliseconds to wait for events before timeout
         * @throws PollTimeoutException when no event occurs during given
         * ammount of time
         * @throws PollException when `poll` returns an error
         */
        void wait(int timeout) {
            for (pollfd &client: clients) {
                client.revents = 0;
            }

            int res = poll(clients, clients_length, timeout);
            if (res == 0) {
                throw PollTimeoutException();
            } else if (res < 0) {
                throw PollException("Error while calling poll");
            }
        }

        /**
         * Inner class to provide range based for loop.
         */
        class iterator {
        private:
            pollfd *fd;
        public:
            iterator(pollfd *fd) : fd(fd) {}

            iterator operator++() noexcept {
                return iterator(++fd);
            }

            bool operator!=(const iterator &other) const noexcept {
                return fd != other.fd;
            }

            const pollfd &operator*() const {
                return *fd;
            }
        };

        /**
         * @return iterator to the first element of events array.
         */
        iterator begin() {
            return Poll::iterator(clients);
        }

        /**
         * @return iterator to the last element of events array.
         */
        iterator end() {
            return Poll::iterator(clients + active_clients_count);
        }
    };
}

#endif //SIK_UDP_POLL_H
