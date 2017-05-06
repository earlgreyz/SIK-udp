#include <arpa/inet.h>
#include "catch.hpp"
#include "../connections.h"

TEST_CASE("Connections add_client adds new clients", "[Connections]") {
    sik::Connections connections;
    std::time_t now = time(0);

    sockaddr_in client_a;
    client_a.sin_family = AF_INET;
    client_a.sin_addr.s_addr = inet_addr("192.168.0.10");
    client_a.sin_port = htons(10012u);
    connections.add_client(client_a, now);
    CHECK(connections.get_clients(now).size() == 1);

    sockaddr_in client_b;
    client_b.sin_family = AF_INET;
    client_b.sin_addr.s_addr = inet_addr("192.168.0.10");
    client_b.sin_port = htons(10013u);
    connections.add_client(client_b, now);
    CHECK(connections.get_clients(now).size() == 2);

    sockaddr_in client_c;
    client_c.sin_family = AF_INET;
    client_c.sin_addr.s_addr = inet_addr("192.168.0.11");
    client_c.sin_port = htons(10013u);
    connections.add_client(client_c, now);
    REQUIRE(connections.get_clients(now).size() == 3);
}

TEST_CASE("Connections add_client adds new connection on same client", "[Connections]") {
    sik::Connections connections;
    std::time_t now = time(0);
    CHECK(connections.get_clients(now).size() == 0);

    sockaddr_in client_a;
    client_a.sin_family = AF_INET;
    client_a.sin_addr.s_addr = inet_addr("192.168.0.10");
    client_a.sin_port = htons(10012u);

    connections.add_client(client_a, now);
    CHECK(connections.get_clients(now).size() == 1);

    connections.add_client(client_a, now);
    CHECK(connections.get_clients(now).size() == 1);

    connections.add_client(client_a, now + 60u);
    CHECK(connections.get_clients(now).size() == 1);

    connections.add_client(client_a, now + 4 * 60u);
    REQUIRE(connections.get_clients(now).size() == 1);
}

TEST_CASE("Connections get_client return correct clients", "[Connections]") {
    sik::Connections connections;
    std::time_t now = time(0);

    sockaddr_in client_a;
    client_a.sin_family = AF_INET;
    client_a.sin_addr.s_addr = inet_addr("192.168.0.10");
    client_a.sin_port = htons(10012u);

    sockaddr_in client_b;
    client_b.sin_family = AF_INET;
    client_b.sin_addr.s_addr = inet_addr("192.168.0.10");
    client_b.sin_port = htons(10013u);

    connections.add_client(client_a, now);
    connections.add_client(client_b, now + 60u);

    auto clients = connections.get_clients(now);
    CHECK((clients.front().sin_addr.s_addr == client_a.sin_addr.s_addr && clients.front().sin_port == client_a.sin_port));
    clients.pop();
    CHECK(clients.size() == 0);

    clients = connections.get_clients(now + 60u);
    CHECK((clients.front().sin_addr.s_addr == client_a.sin_addr.s_addr && clients.front().sin_port == client_a.sin_port));
    clients.pop();
    CHECK((clients.front().sin_addr.s_addr == client_b.sin_addr.s_addr && clients.front().sin_port == client_b.sin_port));
    clients.pop();
    CHECK(clients.size() == 0);

    clients = connections.get_clients(now + 2 * 60u);
    CHECK((clients.front().sin_addr.s_addr == client_a.sin_addr.s_addr && clients.front().sin_port == client_a.sin_port));
    clients.pop();
    CHECK((clients.front().sin_addr.s_addr == client_b.sin_addr.s_addr && clients.front().sin_port == client_b.sin_port));
    clients.pop();
    CHECK(clients.size() == 0);

    clients = connections.get_clients(now + 3 * 60u);
    CHECK((clients.front().sin_addr.s_addr == client_b.sin_addr.s_addr && clients.front().sin_port == client_b.sin_port));
    clients.pop();
    CHECK(clients.size() == 0);
}


TEST_CASE("Connections get_client removes old connections", "[Connections]") {
    sik::Connections connections;
    std::time_t now = time(0);

    sockaddr_in client_a;
    client_a.sin_family = AF_INET;
    client_a.sin_addr.s_addr = inet_addr("192.168.0.10");
    client_a.sin_port = htons(10012u);

    connections.add_client(client_a, now);
    connections.add_client(client_a, now + 4 * 60u);
    CHECK(connections.get_clients(now).size() == 1);
    CHECK(connections.get_clients(now + 2 * 60u + 1u).size() == 0);
    CHECK(connections.get_clients(now).size() == 0);
    REQUIRE(connections.get_clients(now + 5 * 60u).size() == 1);
}
