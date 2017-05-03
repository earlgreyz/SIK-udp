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

TEST_CASE("Message to_bytes returns proper data", "[Message") {
    sik::Message m(654321, 'a', "");
    std::string bytes = m.to_bytes();
    const char * data = bytes.c_str();
    uint64_t * data64 = (uint64_t *)data;
    REQUIRE((uint64_t)be64toh(data64[0]) == 654321u);
}
