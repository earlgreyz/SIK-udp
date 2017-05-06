#include "catch.hpp"
#include "../buffer.h"

TEST_CASE("Buffer is initialized", "[Buffer]") {
    Buffer<int, 4> buffer;
    REQUIRE(buffer.size() == 0);
}

TEST_CASE("Buffer push iserts items", "[Buffer]") {
    Buffer<int, 4> buffer;
    buffer.push(42);
    buffer.push(1);
    REQUIRE(buffer.size() == 2);
}

TEST_CASE("Buffer pop returns data", "[Buffer]") {
    Buffer<int, 4> buffer;
    buffer.push(42);
    buffer.push(1);
    CHECK(buffer.pop() == 42);
    CHECK(buffer.size() == 1);
    CHECK(buffer.pop() == 1);
    REQUIRE(buffer.size() == 0);
}

TEST_CASE("Buffer operator [] returns data", "[Buffer]") {
    Buffer<int, 4> buffer;
    buffer.push(42);
    buffer.push(1);
    CHECK(buffer[0] == 42);
    REQUIRE(buffer[1] == 1);
}

TEST_CASE("Buffer properly cycles", "[Buffer]") {
    Buffer<int, 4> buffer;
    buffer.push(0);
    buffer.push(1);
    buffer.push(2);
    buffer.push(3);
    buffer.push(4);
    CHECK(buffer.size() == 4);
    REQUIRE(buffer[0] == 1);
}

TEST_CASE("Buffer throws std::out_of_range when pop on empty buffer called", "[Buffer]") {
    Buffer<int, 4> buffer;
    CHECK_THROWS_AS(buffer.pop(), std::out_of_range);

    buffer.push(0);
    buffer.pop();
    REQUIRE_THROWS_AS(buffer.pop(), std::out_of_range);
}

TEST_CASE("Buffer throws std::out_of_range when [] index is out of range", "[Buffer]") {
    Buffer<int, 4> buffer;
    CHECK_THROWS_AS(buffer[0], std::out_of_range);
    CHECK_THROWS_AS(buffer[1], std::out_of_range);
    CHECK_THROWS_AS(buffer[2], std::out_of_range);
    CHECK_THROWS_AS(buffer[3], std::out_of_range);
    CHECK_THROWS_AS(buffer[4], std::out_of_range);

    buffer.push(0);
    CHECK_NOTHROW(buffer[0]);
    CHECK_THROWS_AS(buffer[1], std::out_of_range);
    CHECK_THROWS_AS(buffer[2], std::out_of_range);
    CHECK_THROWS_AS(buffer[3], std::out_of_range);
    CHECK_THROWS_AS(buffer[4], std::out_of_range);

    buffer.push(0);
    buffer.push(0);
    CHECK_NOTHROW(buffer[0]);
    CHECK_NOTHROW(buffer[1]);
    CHECK_NOTHROW(buffer[2]);
    CHECK_THROWS_AS(buffer[3], std::out_of_range);
    CHECK_THROWS_AS(buffer[4], std::out_of_range);

    buffer.push(0);
    buffer.push(0);
    CHECK_NOTHROW(buffer[0]);
    CHECK_NOTHROW(buffer[1]);
    CHECK_NOTHROW(buffer[2]);
    CHECK_NOTHROW(buffer[3]);
    CHECK_THROWS_AS(buffer[4], std::out_of_range);
}

TEST_CASE("Buffer begin and end returns proper data", "[Buffer]") {
    Buffer<int, 4> buffer;

    auto b = Buffer<int, 4>::iterator(&buffer, 0);
    CHECK(!(buffer.begin() != b));
    CHECK(!(buffer.begin() != buffer.end()));

    buffer.push(0);
    CHECK(*buffer.begin() == 0);
    CHECK(buffer.begin() != buffer.end());

    buffer.push(1);
    CHECK(*(++buffer.begin()) == 1);

    buffer.push(2);
    buffer.push(3);
    buffer.push(4);

    auto begin = buffer.begin();
    CHECK(*begin == 1);
    CHECK(begin != buffer.end());
    ++begin;
    CHECK(*begin == 2);
    CHECK(begin != buffer.end());
    ++begin;
    CHECK(*begin == 3);
    CHECK(begin != buffer.end());
    ++begin;
    CHECK(*begin == 4);
    CHECK(begin != buffer.end());
    ++begin;
    REQUIRE(!(begin != buffer.end()));
}

TEST_CASE("Buffer range based for loop works", "[Buffer]") {
    Buffer<int, 4> buffer;
    int i;
    for (i = 1; i <= 4; i++) {
        buffer.push(i);
    }
    CHECK(*buffer.begin() == 1);

    i = 1;
    for (int item: buffer) {
        CHECK(item == i);
        i++;
    }
    REQUIRE(i == 5);
}