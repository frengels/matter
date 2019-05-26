#include <catch2/catch.hpp>

#include "matter/component/registry.hpp"
#include "matter/id/default_component_identifier.hpp"
#include "matter/id/id_cache.hpp"
#include "matter/query/primitives/concurrency.hpp"
#include "matter/query/primitives/exclude.hpp"
#include "matter/query/primitives/filter.hpp"
#include "matter/query/primitives/inaccessible.hpp"
#include "matter/query/primitives/optional.hpp"
#include "matter/query/primitives/read.hpp"
#include "matter/query/primitives/require.hpp"
#include "matter/query/primitives/write.hpp"
#include "matter/query/typed_access.hpp"

TEST_CASE("query")
{
    SECTION("concurrency")
    {
        using boost::hana::type_c;
        auto winte = type_c<matter::typed_access<int,
                                                 matter::prim::write,
                                                 matter::prim::exclude>>;
        auto wintr = type_c<matter::typed_access<int,
                                                 matter::prim::write,
                                                 matter::prim::require>>;
        auto winto = type_c<matter::typed_access<int,
                                                 matter::prim::write,
                                                 matter::prim::optional>>;

        auto iintr = type_c<matter::typed_access<int,
                                                 matter::prim::inaccessible,
                                                 matter::prim::require>>;

        SECTION("static")
        {
            static_assert(!matter::can_access_concurrent(wintr, wintr));
            static_assert(matter::can_access_concurrent(winte, wintr));
            static_assert(matter::can_access_concurrent(winte, winto));

            static_assert(matter::can_access_concurrent(wintr, iintr));
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

    SECTION("filter")
    {
        auto reg = matter::registry<
            matter::default_component_identifier<matter::signed_id<int>>>{};
        reg.register_component<int>();
        reg.register_component<float>();

        reg.create<int, float>(
            std::make_tuple(1),
            std::make_tuple(5.0f));               // create [int, float] group
        reg.create<float>(std::make_tuple(5.0f)); // create [float] group
        reg.create<int>(std::make_tuple(3));      // create [int] group

        auto grp_range = reg.group_container().range();

        auto cache = matter::id_cache{reg.component_id<int>(),
                                      reg.component_id<float>()};

        auto matched = 0;

        for (auto grp : grp_range)
        {
            auto filtered_group = matter::filter_group(
                grp,
                cache,
                boost::hana::type_c<
                    matter::typed_access<int,
                                         matter::prim::write,
                                         matter::prim::require>>);

            if (filtered_group) // [int] and [int, float] should match
            {
                ++matched;
            }
        }

        REQUIRE(matched == 2); // the number of matched groups
        matched = 0;

        for (auto grp : grp_range)
        {
            // match all without float, so should be 1 [int]
            auto filtered_group = matter::filter_group(
                grp,
                cache,
                boost::hana::type_c<
                    matter::typed_access<float,
                                         matter::prim::inaccessible,
                                         matter::prim::exclude>>);

            if (filtered_group)
            {
                static_assert(
                    std::is_same_v<
                        matter::empty,
                        std::decay_t<decltype(*std::move(filtered_group))>>);
                ++matched;
            }
        }

        REQUIRE(matched == 1);

        // filter multiple types at the same time
        SECTION("multiple")
        {
            matched = 0;

            for (auto grp : grp_range)
            {
                auto res = matter::filter_group(
                    grp,
                    cache,
                    boost::hana::type_c<
                        matter::typed_access<float,
                                             matter::prim::read,
                                             matter::prim::optional>>,
                    boost::hana::type_c<
                        matter::typed_access<int,
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

            for (auto grp : grp_range)
            {
                // match all with int and without float, so [int]
                using boost::hana::type_c;
                auto res = matter::filter_group(
                    grp,
                    cache,
                    type_c<matter::typed_access<float,
                                                matter::prim::read,
                                                matter::prim::exclude>>,
                    type_c<matter::typed_access<int,
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
    }
}
