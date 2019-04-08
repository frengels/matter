#include <catch2/catch.hpp>

#include "matter/component/component_identifier.hpp"
#include "matter/component/insert_buffer.hpp"

TEST_CASE("insert_buffer")
{
    matter::component_identifier<float, int> ident{};
    auto buf = matter::insert_buffer{ident.ids<float, int>()};
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
