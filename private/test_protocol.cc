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