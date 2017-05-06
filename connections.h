#ifndef SIK_UDP_CLIENTS_H
#define SIK_UDP_CLIENTS_H

#include <netinet/in.h>
#include <ctime>
#include <utility>
#include <map>
#include <queue>

namespace sik {
    static const std::time_t TIMEOUT = 2 * 60;

    class Connections {
    private:
        using Interval = typename std::pair<std::time_t, std::time_t>;

        struct Client {
            /// Client socket address
            sockaddr_in address;
            /// Intervals for which client should receive messages
            std::queue<Interval> connections;

            /**
             * Constructs new client.
             * @param address client address.
             * @param timestamp first interval timestamp.
             */
            Client(sockaddr_in address, std::time_t timestamp)
                    : address(std::move(address)) {
                connections.push(
                        std::make_pair(timestamp, timestamp + TIMEOUT));
            }

            /**
             * Adds new connection or extends the last existing (if intervals
             * would overlap).
             * @param timestamp connected timestamp.
             */
            void add_connection(std::time_t timestamp) noexcept {
                if (connections.back().second >= timestamp) {
                    connections.back().second = timestamp + TIMEOUT;
                } else {
                    connections.push(
                            std::make_pair(timestamp, timestamp + TIMEOUT));
                }
            }
        };

        /**
         * Checks if given time point is between bounds of interval.
         * @param timestamp time point.
         * @param interval interval.
         * @return whether point is between interval bounds.
         */
        inline bool in_bounds(std::time_t timestamp,
                       const Interval &interval) const noexcept {
            return timestamp >= interval.first && timestamp <= interval.second;
        }

        /// Connected clients.
        std::deque<Client> clients;
    public:
        /**
         * Adds client or extends the timeout on existing one.
         * @param address client address
         * @param connection_time time when client connected.
         */
        void add_client(sockaddr_in address, std::time_t connection_time) {
            for (auto &client: clients) {
                if (client.address.sin_addr.s_addr == address.sin_addr.s_addr
                        && client.address.sin_port == address.sin_port) {
                    client.add_connection(connection_time);
                    return;
                }
            }
            clients.push_back(Client(address, connection_time));
        }

        /**
         * Searches for clients with intervals containing timestamp.
         * Performs cleanup removing all intervals with end < timestamp.
         * @return list of clients to send message to
         */
        std::queue<sockaddr_in> get_clients(std::time_t timestamp) {
            std::queue<sockaddr_in> the_clients;
            auto it = clients.begin();
            while (it != clients.end()) {
                auto &client = *it;
                while (client.connections.size() > 0
                       && client.connections.front().second < timestamp) {
                    client.connections.pop();
                }
                if (client.connections.size() == 0) {
                    clients.erase(it++);
                    continue;
                } else if (in_bounds(timestamp, client.connections.front())) {
                    the_clients.push(client.address);
                }
                it++;
            }
            return std::move(the_clients);
        }
    };
}

#endif //SIK_UDP_CLIENTS_H
