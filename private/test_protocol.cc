#include "catch.hpp"
#include "../protocol.h"

TEST_CASE("is_proper_timestamp returns proper value", "[is_proper_timestamp]") {
    CHECK(is_proper_timestamp(0u));
    CHECK(is_proper_timestamp(1));
    CHECK(is_proper_timestamp(42));

    CHECK(is_proper_timestamp(42));
    CHECK(!is_proper_timestamp(71697398400u));
    REQUIRE(!is_proper_timestamp(10000000000000u));
}