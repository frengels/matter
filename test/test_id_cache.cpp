#include <catch2/catch.hpp>

#include "matter/id/component_identifier.hpp"
#include "matter/id/default_component_identifier.hpp"
#include "matter/id/id_cache.hpp"

TEST_CASE("id_cache")
{
    SECTION("identifier_construct")
    {
        // a main identifier
        auto ident =
            matter::default_component_identifier<matter::signed_id<int>>{};
        ident.register_component<int>();
        ident.register_component<float>();

        using boost::hana::type_c;
        // create from hana types
        auto cache = matter::id_cache{ident, type_c<int>, type_c<float>};
        // create from typed ids
        auto cache1 = matter::id_cache{ident.id<float>(), ident.id<int>()};
        // create without any deduction
        auto cache2 =
            matter::id_cache<typename decltype(ident)::id_type, float, int>{
                ident};
        (void) cache2;

        static_assert(matter::is_component_identifier_v<decltype(cache)>);
        static_assert(
            matter::is_component_identifier_for_v<decltype(cache), int>);
        // making this work requires use of sfinae
        static_assert(!matter::is_component_identifier_for_v<
                      decltype(cache),
                      matter::prototype::component>);

        REQUIRE(ident.id<int>() == cache.id<int>());
        REQUIRE(cache.id<float>() == cache1.id<float>());

        REQUIRE(cache.contains<int>());
        REQUIRE(!cache.contains<char>());
    }
}
