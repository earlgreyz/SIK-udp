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
            sockaddr_in address;
            std::queue<Interval> connections;

            Client(sockaddr_in address, timestamp_t timestamp)
                    : address(std::move(address)) {
                connections.push(std::make_pair(timestamp, timestamp + TIMEOUT));
            }

            void add_connection(std::time_t timestamp) noexcept {
                if (connections.back().second >= timestamp) {
                    connections.back().second = timestamp + TIMEOUT;
                } else {
                    connections.push(
                            std::make_pair(timestamp, timestamp + TIMEOUT));
                }
            }
        };

        bool in_bounds(std::time_t timestamp, const Interval &interval) const noexcept {
            return timestamp >= interval.first && timestamp <= interval.second;
        }

        std::deque<Client> clients;
    public:
        /**
         * Adds client with current timestamp and timeout.
         * @param address
         */
        void add_client(sockaddr_in address) {
            std::time_t now = std::time(0);
            for (auto& client: clients) {
                if (client.address.sin_addr.s_addr == address.sin_addr.s_addr
                        && client.address.sin_port == address.sin_port) {
                    client.add_connection(now);
                    return;
                }
            }
            clients.push_back(Client(address, now));
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
