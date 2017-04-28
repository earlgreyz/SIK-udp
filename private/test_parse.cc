#include "catch.hpp"
#include "../parse.h"


TEST_CASE("parse_port returns proper data", "[parse_port]") {
    CHECK(parse_port("1") == 1);
    CHECK(parse_port("42") == 42);
    REQUIRE(parse_port("65535") == 65535);
}

TEST_CASE("parse_port throws errors on invalid data", "[parse_port]") {
    CHECK_THROWS_AS(parse_port("0"), ParseException);
    CHECK_THROWS_AS(parse_port("-1"), ParseException);
    CHECK_THROWS_AS(parse_port("65536"), ParseException);
    CHECK_THROWS_AS(parse_port("1000000000000000"), ParseException);

    CHECK_THROWS_AS(parse_port("abc"), ParseException);
    CHECK_THROWS_AS(parse_port("42abc"), ParseException);
    REQUIRE_THROWS_AS(parse_port("42 42"), ParseException);
}