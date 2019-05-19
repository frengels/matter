#include <catch2/catch.hpp>

#include "matter/component/component_identifier.hpp"
#include "matter/component/group.hpp"
#include "matter/component/group_container.hpp"
#include "matter/component/group_slice.hpp"

TEST_CASE("group_container")
{

    matter::component_identifier<matter::unsigned_id<std::size_t>> ident;
    ident.register_type<int>();
    ident.register_type<float>();
    ident.register_type<char>();
    ident.register_type<double>();

    matter::group_container<matter::unsigned_id<std::size_t>> cont;

    // fill up with groups
    cont.try_emplace(ident.ids<char>());

    CHECK(1 == cont.range().size());
    CHECK(1 == cont.size());
    cont.try_emplace(ident.ids<float>());
    CHECK(2 == cont.range().size());
    CHECK(2 == cont.size());
    cont.try_emplace_group(ident.ids<int, float, char, double>());
    CHECK(3 == cont.range().size());
    CHECK(6 == cont.size());
    cont.try_emplace_group(ident.ids<float>()); // is already inserted
    CHECK(3 == cont.range().size());
    CHECK(6 == cont.size());

    // get here because can get invalidated before
    auto ifcdgrp_opt = cont.find_group(ident.ids<int, float, char, double>());
    CHECK(ifcdgrp_opt);
    auto ifcdgrp = *std::move(ifcdgrp_opt);
    // old groups possibly invalidated here

    ifcdgrp   = *cont.find_group(ident.ids<int, float, char, double>());
    auto fgrp = *cont.find_group(ident.ids<float>());

    auto fgrp_it = cont.find(ident.ordered_ids<float>());
    CHECK(fgrp_it != cont.end());

    // obtain the group a different way
    auto fgrp2 = *matter::make_group(*fgrp_it, ident.ids<float>());
    CHECK(fgrp == fgrp2);

    // obtain a slice as well
    auto fgrp_slice = *matter::make_group_slice(*fgrp_it, ident.ids<float>());
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
        auto ids         = ident.ids<double>();
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
