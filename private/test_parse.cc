#include "catch.hpp"
#include "../parse.h"


TEST_CASE("parse_port returns proper data", "[parse_port]") {
    CHECK(parse_port("1") == 1);
    CHECK(parse_port("42") == 42);
    REQUIRE(parse_port("65535") == 65535);
}

TEST_CASE("parse_port throws errors on invalid input", "[parse_port]") {
    CHECK_THROWS_AS(parse_port("0"), ParseException);
    CHECK_THROWS_AS(parse_port("-1"), ParseException);
    CHECK_THROWS_AS(parse_port("65536"), ParseException);
    CHECK_THROWS_AS(parse_port("1000000000000000"), ParseException);

    CHECK_THROWS_AS(parse_port("abc"), ParseException);
    CHECK_THROWS_AS(parse_port("42abc"), ParseException);
    REQUIRE_THROWS_AS(parse_port("42 42"), ParseException);
}

TEST_CASE("parse_character returns proper data", "[parse_character]") {
    CHECK(parse_character("a") == 'a');
    CHECK(parse_character("0") == '0');
    CHECK(parse_character("!") == '!');
    REQUIRE(parse_character("\127") == '\127');
}

TEST_CASE("parse_character throws errors on invalid input", "[parse_character]") {
    CHECK_THROWS_AS(parse_character(""), ParseException);
    CHECK_THROWS_AS(parse_character("ą"), ParseException);
    REQUIRE_THROWS_AS(parse_character("ab"), ParseException);
}

TEST_CASE("parse_timestamp returns proper data", "[parse_timestamp]") {
    CHECK(parse_timestamp("0") == 0);
    CHECK(parse_timestamp("42") == 42);
    REQUIRE(parse_timestamp("18446744073709551615") == 18446744073709551615u);
}

TEST_CASE("parse_timestamp throws errors on invalid input", "[parse_timestamp]") {
    CHECK_THROWS_AS(parse_timestamp("-1"), ParseException);
    REQUIRE_THROWS_AS(parse_timestamp("18446744073709551616"), ParseException);
    REQUIRE_THROWS_AS(parse_timestamp("100000000000000000000"), ParseException);

    CHECK_THROWS_AS(parse_timestamp("abc"), ParseException);
    CHECK_THROWS_AS(parse_timestamp("42abc"), ParseException);
    REQUIRE_THROWS_AS(parse_timestamp("42 42"), ParseException);
}
