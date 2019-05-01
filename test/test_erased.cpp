#include <catch2/catch.hpp>

#include <string>

#include "matter/component/component_identifier.hpp"
#include "matter/storage/erased_storage.hpp"
#include "matter/util/erased.hpp"

struct int_comp
{
    int i;
    constexpr int_comp(int i) : i{i}
    {}
};

struct float_comp
{
    float f;
    constexpr float_comp(float f) : f{f}
    {}
};

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

TEST_CASE("erased_storage")
{
    matter::component_identifier<std::size_t> ident;
    ident.register_type<int_comp>();
    ident.register_type<float_comp>();

    matter::erased_storage store{ident.id<int_comp>()};

    SECTION("push back")
    {
        int_comp icomp1{50};

        store.push_back(
            matter::erased_component{ident.id<int_comp>().value(), &icomp1});

        auto  er_comp    = store[0];
        auto& icomp1_ref = *static_cast<int_comp*>(er_comp.get());
        // check insertion went well
        CHECK(icomp1_ref.i == 50);

        int_comp icomp2{42};

        store.push_back(
            matter::erased_component{ident.id<int_comp>().value(), &icomp2});

        er_comp                = store[0];
        auto& icomp1_ref_again = *static_cast<int_comp*>(er_comp.get());
        // check it's still at first position
        CHECK(icomp1_ref_again.i == 50);

        er_comp          = store[1];
        auto& icomp2_ref = *static_cast<int_comp*>(er_comp.get());
        // check icomp2 was copied inserted correctly
        CHECK(icomp2_ref.i == 42);

        SECTION("erase")
        {
            store.erase(0);

            er_comp                = store[0];
            auto& icomp2_ref_again = *static_cast<int_comp*>(er_comp.get());
            CHECK(icomp2_ref_again.i == 42);
        }
    }
}
