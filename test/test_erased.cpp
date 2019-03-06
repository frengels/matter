#include <catch2/catch.hpp>

#include <string>

#include "matter/util/erased.hpp"

struct our_test
{
    int& i;

    // just to make this struct a bit bigger
    char c[16]{};

    constexpr our_test(int& i) : i{i}
    {
        i += 2;
    }

    ~our_test()
    {
        i += 5;
    }
};

TEST_CASE("erased")
{
    SECTION("construct")
    {
        int i{10};

        matter::erased er{std::in_place_type_t<our_test>{}, i};
        CHECK(i == 12);
    }

    SECTION("destructor")
    {
        int i{10};

        {
            matter::erased er = matter::make_erased<our_test>(i);
            i                 = 10;
            // at the end of scope destruction should increment by 5
        }

        CHECK(i == 15);
    }

    SECTION("get")
    {
        int  i  = 0;
        auto er = matter::make_erased<our_test>(i);

        // i is now 12
        int& i_ref = er.get<our_test>().i;
        CHECK(i_ref == i);

        const int& i_const_ref = er.get<our_test>().i;
        CHECK(i_const_ref == i);
    }

    SECTION("empty")
    {
        auto er = matter::make_erased<std::string>("Hello world");
        CHECK(!er.empty());

        er.clear();
        CHECK(er.empty());
    }

    SECTION("move")
    {
        auto er = matter::make_erased<std::string>("hello world");
        CHECK(!er.empty());

        matter::erased er_move_con{std::move(er)};
        CHECK(!er_move_con.empty());
        CHECK(er.empty());

        er = std::move(er_move_con);
        CHECK(!er.empty());
        CHECK(er_move_con.empty());

        CHECK(er.get<std::string>().compare("hello world") == 0);
    }
}
