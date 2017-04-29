#include "catch.hpp"
#include "../parse.h"

TEST_CASE("parse_port returns proper data", "[parse_port]") {
    CHECK(sik::parse_port("1") == 1);
    CHECK(sik::parse_port("42") == 42);
    REQUIRE(sik::parse_port("65535") == 65535);
}

TEST_CASE("parse_port throws errors on invalid input", "[parse_port]") {
    CHECK_THROWS_AS(sik::parse_port("0"), sik::ParseException);
    CHECK_THROWS_AS(sik::parse_port("-1"), sik::ParseException);
    CHECK_THROWS_AS(sik::parse_port("65536"), sik::ParseException);
    CHECK_THROWS_AS(sik::parse_port("1000000000000000"), sik::ParseException);

    CHECK_THROWS_AS(sik::parse_port("abc"), sik::ParseException);
    CHECK_THROWS_AS(sik::parse_port("42abc"), sik::ParseException);
    REQUIRE_THROWS_AS(sik::parse_port("42 42"), sik::ParseException);
}

TEST_CASE("parse_character returns proper data", "[parse_character]") {
    CHECK(sik::parse_character("a") == 'a');
    CHECK(sik::parse_character("0") == '0');
    CHECK(sik::parse_character("!") == '!');
    REQUIRE(sik::parse_character("\127") == '\127');
}

TEST_CASE("parse_character throws errors on invalid input", "[parse_character]") {
    CHECK_THROWS_AS(sik::parse_character(""), sik::ParseException);
    CHECK_THROWS_AS(sik::parse_character("ą"), sik::ParseException);
    REQUIRE_THROWS_AS(sik::parse_character("ab"), sik::ParseException);
}

TEST_CASE("parse_timestamp returns proper data", "[parse_timestamp]") {
    CHECK(sik::parse_timestamp("0") == 0);
    CHECK(sik::parse_timestamp("42") == 42);
    REQUIRE(sik::parse_timestamp("71697398399") == 71697398399u);
}

TEST_CASE("parse_timestamp throws errors on invalid input", "[parse_timestamp]") {
    CHECK_THROWS_AS(sik::parse_timestamp("-1"), sik::ParseException);
    CHECK_THROWS_AS(sik::parse_timestamp("71697398400"), sik::ParseException);
    CHECK_THROWS_AS(sik::parse_timestamp("18446744073709551616"), sik::ParseException);
    CHECK_THROWS_AS(sik::parse_timestamp("100000000000000000000"), sik::ParseException);

    CHECK_THROWS_AS(sik::parse_timestamp("abc"), sik::ParseException);
    CHECK_THROWS_AS(sik::parse_timestamp("42abc"), sik::ParseException);
    REQUIRE_THROWS_AS(sik::parse_timestamp("42 42"), sik::ParseException);
}
