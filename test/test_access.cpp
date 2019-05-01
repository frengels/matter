#include <catch2/catch.hpp>

#include "matter/access/read.hpp"
#include "matter/access/type_traits.hpp"
#include "matter/access/write.hpp"
#include "matter/component/registry.hpp"

TEST_CASE("access")
{
    SECTION("static_assertions")
    {
        static_assert(std::is_same_v<
                      matter::read<int>,
                      matter::make_access_result_t<matter::read<int>::meta_type<
                          matter::registry<std::size_t>>>>);

        static_assert(matter::is_access_v<matter::read<int>,
                                          matter::registry<std::size_t>>);
        static_assert(matter::is_access_v<matter::read<float>,
                                          matter::registry<std::size_t>>);

        static_assert(
            matter::is_meta_access_v<
                matter::read<float>::meta_type<matter::registry<std::size_t>>>);
        static_assert(
            matter::is_meta_access_v<
                matter::read<float>::meta_type<matter::registry<std::size_t>>>);
    }

    matter::registry<std::size_t> reg;

    reg.register_component<int>();
    reg.register_component<float>();

    reg.create<int, float>(std::forward_as_tuple(5),
                           std::forward_as_tuple(15.f));

    auto acc_view = reg.view<matter::read<int>, matter::read<float>>();

    acc_view.for_each([](auto, auto) {});

    SECTION("modify")
    {
        auto write_view = reg.view<matter::write<float>, matter::write<int>>();

        write_view.for_each([](auto fwrite, auto iwrite) {
            fwrite.stage(20.0f);
            iwrite.stage(10);
        });
    }
}
