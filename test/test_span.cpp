#include <catch2/catch.hpp>

#include <vector>

#include "matter/container/span.hpp"

TEST_CASE("span")
{
    std::array<int, 5> arr{1, 2, 3, 4, 5};
    matter::span       s{arr};

    SECTION("iterators")
    {
        // normal
        REQUIRE(*s.begin() == 1);
        REQUIRE(*(s.begin() + 1) == 2);
        REQUIRE(*(s.end() - 1) == 5);

        // reverse
        REQUIRE(*s.rbegin() == 5);
        REQUIRE(*(s.rbegin() + 1) == 4);
        REQUIRE(*(s.rend() - 1) == 1);
    }

    SECTION("front_back")
    {
        REQUIRE(s.front() == 1);
        REQUIRE(s.back() == 5);
    }

    SECTION("size")
    {
        REQUIRE(s.size() == 5);
        REQUIRE(!s.empty());
    }

    SECTION("subspan")
    {
        auto first_sub = s.first(3);
        REQUIRE(first_sub.front() == 1);
        REQUIRE(first_sub.back() == 3);
        REQUIRE(first_sub.size() == 3);

        auto last_sub = s.last(3);
        REQUIRE(last_sub.back() == 5);
        REQUIRE(last_sub.front() == 3);
        REQUIRE(last_sub.size() == 3);

        auto offs_sub = s.subspan(2);
        REQUIRE(offs_sub.front() == 3);
        REQUIRE(offs_sub.back() == 5);
        REQUIRE(offs_sub.size() == 3);

        auto cnt_sub = s.subspan(1, 3);
        REQUIRE(cnt_sub.front() == 2);
        REQUIRE(cnt_sub.back() == 4);
        REQUIRE(cnt_sub.size() == 3);
    }

    std::vector v{1, 2, 3, 4, 5, 6, 7, 8, 9};
}