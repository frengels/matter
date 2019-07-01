#include <catch2/catch.hpp>

#include "matter/component/registry.hpp"
#include "matter/id/default_component_identifier.hpp"
#include "matter/id/id_cache.hpp"
#include "matter/query/entities.hpp"
#include "matter/query/primitives/concurrency.hpp"
#include "matter/query/processor.hpp"
#include "matter/query/type_query.hpp"
#include "matter/query/type_traits.hpp"
#include "matter/world.hpp"

TEST_CASE("query")
{
    SECTION("concurrency")
    {
        using boost::hana::type_c;
        auto winte = type_c<matter::type_query<int,
                                               matter::prim::write,
                                               matter::prim::exclude>>;
        auto wintr = type_c<matter::type_query<int,
                                               matter::prim::write,
                                               matter::prim::require>>;
        auto winto = type_c<matter::type_query<int,
                                               matter::prim::write,
                                               matter::prim::optional>>;

        auto iintr = type_c<matter::type_query<int,
                                               matter::prim::inaccessible,
                                               matter::prim::require>>;

        SECTION("static")
        {
            static_assert(!matter::can_access_concurrent(wintr, wintr));
            static_assert(matter::can_access_concurrent(winte, wintr));
            static_assert(matter::can_access_concurrent(winte, winto));

            static_assert(matter::can_access_concurrent(wintr, iintr));

            static_assert(matter::traits::has_query_category(
                type_c<matter::entities<matter::write<int>>>));
        }

        SECTION("dynamic")
        {
            auto ident =
                matter::default_component_identifier<matter::signed_id<int>>{};
            ident.register_component<int>();

            auto winte_descr =
                matter::component_query_description{ident, winte};
            auto wintr_descr =
                matter::component_query_description{ident, wintr};
            auto winto_descr =
                matter::component_query_description{ident, winto};
            auto iintr_descr =
                matter::component_query_description{ident, iintr};

            REQUIRE(!wintr_descr.can_access_concurrent(wintr_descr));
            REQUIRE(winte_descr.can_access_concurrent(wintr_descr));
            REQUIRE(winte_descr.can_access_concurrent(winto_descr));
            REQUIRE(iintr_descr.can_access_concurrent(wintr_descr));
        }
    }

    SECTION("type_traits")
    {
        static_assert(matter::traits::is_entity_query(
            boost::hana::type_c<matter::entities<matter::write<int>>>));
        static_assert(
            !matter::traits::is_entity_query(boost::hana::type_c<int>));

        static_assert(matter::traits::is_query_category(
            boost::hana::type_c<matter::entity_query_tag>));

        static_assert(
            matter::traits::is_query_category<matter::entity_query_tag>());
    }

    SECTION("filter")
    {
        // auto reg = matter::registry<
        //     matter::default_component_identifier<matter::signed_id<int>>>{};

        auto w = matter::world{};
        w.register_component<int>();
        w.register_component<float>();

        w.create_entity<int, float>(1, float{});
        w.create_entity<float>(5.0f);
        w.create_entity<int>(3);

        auto cache =
            matter::id_cache{w.component_id<int>(), w.component_id<float>()};

        auto matched = 0;

        for (auto grp : w.group_range())
        {
            auto filtered_group = matter::filter_group(
                grp,
                cache,
                boost::hana::type_c<matter::type_query<int,
                                                       matter::prim::write,
                                                       matter::prim::require>>);

            if (filtered_group) // [int] and [int, float] should match
            {
                ++matched;
            }
        }

        REQUIRE(matched == 2); // the number of matched groups
        matched = 0;

        for (auto grp : w.group_range())
        {
            // match all without float, so should be 1 [int]
            auto filtered_group = matter::filter_group(
                grp,
                cache,
                boost::hana::type_c<
                    matter::type_query<float,
                                       matter::prim::inaccessible,
                                       matter::prim::exclude>>);

            if (filtered_group)
            {
                static_assert(
                    std::is_same_v<
                        matter::empty,
                        std::tuple_element_t<0,
                                             std::decay_t<decltype(*std::move(
                                                 filtered_group))>>>);
                ++matched;
            }
        }

        REQUIRE(matched == 1);

        // filter multiple types at the same time
        SECTION("multiple")
        {
            matched = 0;

            for (auto grp : w.group_range())
            {
                auto res = matter::filter_group(
                    grp,
                    cache,
                    boost::hana::type_c<
                        matter::type_query<float,
                                           matter::prim::read,
                                           matter::prim::optional>>,
                    boost::hana::type_c<
                        matter::type_query<int,
                                           matter::prim::write,
                                           matter::prim::require>>);

                // should match on [int, float] and [int]
                if (res)
                {
                    ++matched;
                }
            }

            REQUIRE(matched == 2);

            matched = 0;

            for (auto grp : w.group_range())
            {
                // match all with int and without float, so [int]
                using boost::hana::type_c;
                auto res = matter::filter_group(
                    grp,
                    cache,
                    type_c<matter::type_query<float,
                                              matter::prim::read,
                                              matter::prim::exclude>>,
                    type_c<matter::type_query<int,
                                              matter::prim::read,
                                              matter::prim::require>>);

                if (res)
                {
                    ++matched;
                    auto [fstore, istore] = *std::move(res);
                    REQUIRE(istore[0] == 3);
                }
            }

            REQUIRE(matched == 1);
        }

        SECTION("process_entity_query")
        {
            using boost::hana::type_c;

            auto matched = 0;

            auto eq = matter::entities{
                type_c<matter::write<int>>}; // match [int] and [int, float]

            for (auto [i] : matter::process_query(eq, w))
            {
                ++matched;
            }

            REQUIRE(matched == 2);

            matched = 0;

            auto eq1 = matter::entities{type_c<matter::read<float>>,
                                        type_c<matter::opt_read<int>>};

            // should match 2 groups, [float] and [float, int]

            auto opt_present = 0;

            for (auto [f_store, i_store] : matter::process_query(eq1, w))
            {
                ++matched;

                if (i_store)
                {
                    ++opt_present;
                }
            }

            REQUIRE(matched == 2);
            REQUIRE(opt_present == 1);
        }
    }
}
