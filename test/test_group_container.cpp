#include <catch2/catch.hpp>

#include "matter/component/group.hpp"
#include "matter/component/group_container.hpp"
#include "matter/component/group_slice.hpp"
#include "matter/id/default_component_identifier.hpp"

TEST_CASE("group_container")
{

    matter::default_component_identifier<matter::unsigned_id<std::size_t>>
        ident;
    ident.register_component<int>();
    ident.register_component<float>();
    ident.register_component<char>();
    ident.register_component<double>();

    matter::group_container<matter::unsigned_id<std::size_t>> cont;

    // fill up with groups
    cont.try_emplace(ident.component_ids<char>());

    CHECK(1 == cont.range().size());
    CHECK(1 == cont.size());
    cont.try_emplace(ident.component_ids<float>());
    CHECK(2 == cont.range().size());
    CHECK(2 == cont.size());
    cont.try_emplace_group(ident.component_ids<int, float, char, double>());
    CHECK(3 == cont.range().size());
    CHECK(6 == cont.size());
    cont.try_emplace_group(ident.component_ids<float>()); // is already inserted
    CHECK(3 == cont.range().size());
    CHECK(6 == cont.size());

    // get here because can get invalidated before
    auto ifcdgrp_opt =
        cont.find_group(ident.component_ids<int, float, char, double>());
    CHECK(ifcdgrp_opt);
    auto ifcdgrp = *std::move(ifcdgrp_opt);
    // old groups possibly invalidated here

    ifcdgrp = *cont.find_group(ident.component_ids<int, float, char, double>());
    auto fgrp = *cont.find_group(ident.component_ids<float>());

    auto fgrp_it = cont.find(ident.ordered_component_ids<float>());
    CHECK(fgrp_it != cont.end());

    // obtain the group a different way
    auto fgrp2 = *matter::make_group(*fgrp_it, ident.component_ids<float>());
    CHECK(fgrp == fgrp2);

    // obtain a slice as well
    auto fgrp_slice =
        *matter::make_group_slice(*fgrp_it, ident.component_ids<float>());
    CHECK(fgrp2 == fgrp_slice);

    SECTION("contains")
    {
        CHECK(fgrp.contains<float>());
    }

    // fill groups with values
    fgrp.emplace_back(std::forward_as_tuple(5.0f));
    ifcdgrp.emplace_back(5, 5.f, 'i', 5.0);

    SECTION("size")
    {
        auto rng4 = cont.range(4);
        auto rng1 = cont.range(1);

        CHECK(rng1.size() == 2);
        CHECK(rng4.size() == 1);

        CHECK(cont.groups_size() == 4);
    }

    SECTION("find")
    {
        auto ids         = ident.component_ids<double>();
        auto ordered_ids = matter::ordered_typed_ids{ids};

        auto ordered_untyped_ids = matter::ordered_untyped_ids{ordered_ids};

        SECTION("typed")
        {
            CHECK(cont.find(ordered_ids) == cont.end());
            CHECK(!bool(cont.find_group(ids, ordered_ids)));
        }

        SECTION("untyped")
        {
            CHECK(cont.find(ordered_untyped_ids) == cont.end());
            CHECK(!bool(cont.find_group(ordered_untyped_ids)));
        }
    }
}
