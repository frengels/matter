#include <catch2/catch.hpp>

#include "matter/component/component_identifier.hpp"
#include "matter/component/typed_id.hpp"

template<typename T, std::size_t Id = 0>
using static_typed_id = matter::typed_id<std::size_t, T, Id>;

template<typename T>
using runtime_typed_id =
    matter::typed_id<std::size_t, T, std::numeric_limits<std::size_t>::max()>;

TEST_CASE("typed_id")
{
    SECTION("basics")
    {
        // do typed_ids get identified correctly
        static_assert(matter::is_typed_id_v<static_typed_id<int>>);
        static_assert(matter::is_typed_id_v<runtime_typed_id<int>>);

        // is static and runtime correct
        static_assert(static_typed_id<float>::is_static());
        static_assert(!runtime_typed_id<float>::is_static());
    }

    SECTION("contains")
    {
        matter::component_identifier<char, int, float, short, long> ident{};
        ident.register_type<unsigned char>();
        ident.register_type<double>();
        ident.register_type<unsigned int>();

        SECTION("same size")
        {
            auto unordered1  = ident.ids<char, short, float>();
            auto unordered2  = ident.ids<short, char, float>();
            auto rt_unorder1 = ident.ids<unsigned char, double, char>();
            auto rt_unorder2 = ident.ids<double, unsigned char, char>();

            auto ordered1  = ident.ordered_ids<float, short, char>();
            auto rt_order1 = ident.ordered_ids<char, unsigned char, double>();

            // static types contains
            CHECK(ordered1.contains(matter::ordered_typed_ids{unordered1}));
            CHECK(ordered1.contains(matter::ordered_typed_ids{unordered2}));

            CHECK(!ordered1.contains(matter::ordered_typed_ids{rt_unorder1}));
            CHECK(!ordered1.contains(matter::ordered_typed_ids{rt_unorder2}));

            // runtime types contains
            CHECK(rt_order1.contains(matter::ordered_typed_ids{rt_unorder1}));
            CHECK(rt_order1.contains(matter::ordered_typed_ids{rt_unorder2}));

            CHECK(!rt_order1.contains(matter::ordered_typed_ids{unordered1}));
            CHECK(!rt_order1.contains(matter::ordered_typed_ids{unordered2}));
        }

        SECTION("smaller")
        {
            auto unordered1 = ident.ids<char, short>();
            auto no_match   = ident.ids<int, short, float>();

            auto ordered =
                ident.ordered_ids<char, double, unsigned int, float, short>();

            CHECK(ordered.contains(matter::ordered_typed_ids{unordered1}));
            CHECK(!ordered.contains(matter::ordered_typed_ids{no_match}));
        }
    }
}
