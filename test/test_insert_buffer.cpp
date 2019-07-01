#include <catch2/catch.hpp>

#include "matter/component/insert_buffer.hpp"
#include "matter/id/default_component_identifier.hpp"

TEST_CASE("insert_buffer")
{
    matter::default_component_identifier<matter::unsigned_id<std::size_t>,
                                         float,
                                         int>
         ident{};
    auto buf = matter::insert_buffer{
        matter::unordered_typed_ids{ident.template component_id<float>(),
                                    ident.template component_id<int>()}};
    buf.reserve(10000);

    SECTION("emplace_back")
    {
        buf.emplace_back(std::forward_as_tuple(5.5f),
                         std::forward_as_tuple(50));
        CHECK(buf.size() == 1);

        buf.emplace_back(5.5f, 50);
        CHECK(buf.size() == 2);
    }
}
