#include <catch2/catch.hpp>

#include "matter/iterator/begin.hpp"
#include "matter/iterator/end.hpp"

namespace my
{
struct adlable
{
    friend int* begin(adlable&) noexcept
    {
        return nullptr;
    }

    friend int* end(adlable&) noexcept
    {
        return nullptr;
    }
};
} // namespace my

TEST_CASE("begin")
{
    SECTION("array")
    {
        SECTION("nonconst")
        {
            int    arr[]  = {0, 1, 2, 3, 4, 5};
            auto&& it     = matter::begin(arr);
            auto&& end_it = matter::end(arr);
            static_assert(std::is_same_v<int*, decltype(matter::begin(arr))>);
            static_assert(std::is_same_v<int*, decltype(matter::end(arr))>);
            static_assert(noexcept(matter::begin(arr)));
            static_assert(noexcept(matter::end(arr)));
            REQUIRE(*it == 0);
            REQUIRE(*(end_it - 1) == 5);
        }

        SECTION("const")
        {
            const int arr[]  = {0, 1, 2, 3, 4, 5};
            auto&&    it     = matter::begin(arr);
            auto&&    end_it = matter::end(arr);
            static_assert(
                std::is_same_v<const int*, decltype(matter::begin(arr))>);
            static_assert(
                std::is_same_v<const int*, decltype(matter::end(arr))>);
            REQUIRE(*it == 0);
            REQUIRE(*(end_it - 1) == 5);
        }

        SECTION("rvalue")
        {
            // cannot call on rvalue array
            int arr[] = {0, 1, 2, 3, 4, 5};
            static_assert(!std::is_invocable_v<decltype(matter::begin),
                                               decltype(std::move(arr))>);
            static_assert(!std::is_invocable_v<decltype(matter::end),
                                               decltype(std::move(arr))>);
        }
    }

    SECTION("initializer_list")
    {
        SECTION("rvalue")
        {
            static_assert(!std::is_invocable_v<decltype(matter::begin),
                                               std::initializer_list<int>&&>);
            static_assert(!std::is_invocable_v<decltype(matter::end),
                                               std::initializer_list<int>&&>);
        }
    }

    SECTION("member")
    {
        std::vector<int> v{0, 1, 2, 3, 4, 5};

        auto it     = matter::begin(v);
        auto end_it = matter::end(v);

        REQUIRE(*it == 0);
        REQUIRE(*(end_it - 1) == 5);
    }

    SECTION("adl")
    {
        auto adl = my::adlable{};

        auto it     = matter::begin(adl);
        auto end_it = matter::end(adl);
        static_assert(noexcept(matter::begin(adl)));
        static_assert(noexcept(matter::end(adl)));
        REQUIRE(it == nullptr);
        REQUIRE(end_it == nullptr);
    }
}
