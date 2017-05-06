#include "catch.hpp"
#include "../protocol.h"

TEST_CASE("is_proper_timestamp returns proper value", "[is_proper_timestamp]") {
    CHECK(sik::is_proper_timestamp(0u));
    CHECK(sik::is_proper_timestamp(1));
    CHECK(sik::is_proper_timestamp(42));

    CHECK(sik::is_proper_timestamp(42));
    CHECK(!sik::is_proper_timestamp(71697398400u));
    REQUIRE(!sik::is_proper_timestamp(10000000000000u));
}

TEST_CASE("Message standard constructor works", "[Message]") {
    sik::Message m(654321u, 'a', "");
    CHECK_FALSE(m.has_message());
    CHECK(m.get_timestamp() == 654321u);
    CHECK(m.get_character() == 'a');
    CHECK(m.get_message() == "");

    sik::Message n(654321u, 'b', "Ala ma kota");
    CHECK(n.has_message());
    CHECK(n.get_timestamp() == 654321u);
    CHECK(n.get_character() == 'b');
    REQUIRE(n.get_message() == "Ala ma kota");
}

TEST_CASE("Message from bytes constructor works", "[Message]") {
    char b[9];
    ((uint64_t *)b)[0] = htobe64(654321u);
    b[8] = 'a';
    sik::Message m(b, sizeof(b));

    CHECK_FALSE(m.has_message());
    CHECK(m.get_timestamp() == 654321u);
    CHECK(m.get_character() == 'a');
    CHECK(m.get_message() == "");

    char c[] = "        bAla ma kota";
    ((uint64_t *)c)[0] = htobe64(654321u);
    sik::Message n(c, sizeof(c));
    CHECK(n.has_message());
    CHECK(n.get_timestamp() == 654321u);
    CHECK(n.get_character() == 'b');
    REQUIRE(n.get_message() == "Ala ma kota");
}

TEST_CASE("Message constructors throws errors when wrong bytes format", "[Message]") {
    for (std::size_t i = 1; i < 8u; i++) {
        char c[i];
        CHECK_THROWS_AS(sik::Message(c, sizeof(c)), std::invalid_argument);
    }

    char b[8];
    ((uint64_t *)b)[0] = htobe64(654321u);
    REQUIRE_THROWS_AS(sik::Message(b, sizeof(b)), std::invalid_argument);
}

TEST_CASE("Message constructors throws errors when wrong timestamp", "[Message]") {
    REQUIRE_THROWS_AS(sik::Message(71697398400u, 'a', "Ala"), std::invalid_argument);
    char b[9];
    ((uint64_t *)b)[0] = htobe64(71697398400u);
    b[8] = 'a';
    REQUIRE_THROWS_AS(sik::Message(b, sizeof(b)), std::invalid_argument);
}

TEST_CASE("Message to_bytes returns proper data", "[Message]") {
    sik::Message m(654321u, 'a', "");
    std::string bytes = m.to_bytes();
    CHECK(bytes.length() == 9u);
    const char * data = bytes.c_str();
    uint64_t * data64 = (uint64_t *)data;
    CHECK((uint64_t)be64toh(data64[0]) == 654321u);
    REQUIRE(bytes[8] == 'a');
}

TEST_CASE("Message to_bytes returns data proper for bytes constructor", "[Message]") {
    sik::Message m(654321u, 'a', "Ala ma kota");
    std::string bytes = m.to_bytes();
    const char * data = bytes.c_str();
    sik::Message n(data, bytes.length());
    CHECK(m.get_timestamp() == n.get_timestamp());
    CHECK(m.get_character() == n.get_character());
    CHECK(m.has_message() == n.has_message());
    REQUIRE(m.get_message() == n.get_message());
}

TEST_CASE("Message ostream << operator returns proper data", "[Message]") {
    std::stringstream ss;
    sik::Message m(654321u, 'a', "Ala ma kota");
    ss << m;
    REQUIRE(ss.str() == "654321 a Ala ma kota");
}