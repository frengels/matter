#include <catch2/catch.hpp>

#include "matter/storage/sparse_vector.hpp"
#include "matter/storage/sparse_vector_storage.hpp"
#include "matter/storage/traits.hpp"

struct my_component
{
    int i;

    constexpr my_component(int i) : i{i}
    {}
};

TEST_CASE("sparse_vector_storage")
{
    matter::sparse_vector_storage<uint32_t, my_component> my_storage;

    static_assert(
        matter::is_storage_component_constructible_v<decltype(my_storage),
                                                     my_component,
                                                     int>,
        "my_storage does not support the component constructible concept");
    static_assert(matter::is_storage_accessible_v<decltype(my_storage)>,
                  "my_storage does not offer access to components by id");
    static_assert(matter::is_storage_iterable_v<decltype(my_storage)>);
    static_assert(matter::is_storage_const_iterable_v<decltype(my_storage)>);
    static_assert(matter::is_storage_reverse_iterable_v<decltype(my_storage)>);
    static_assert(
        matter::is_storage_const_reverse_iterable_v<decltype(my_storage)>);
    static_assert(matter::is_storage_sized_v<decltype(my_storage)>);
    static_assert(matter::is_storage_accessible_v<decltype(my_storage)>);
    static_assert(matter::is_storage_const_accessible_v<decltype(my_storage)>);

    SECTION("construct")
    {
        auto index = GENERATE(as<uint32_t>(), 0, 1, 3, 5, 9, 100, 80);

        SECTION("values")
        {
            auto val = GENERATE(5, 4, 3, 9, 10, 300, 9001);

            REQUIRE(!my_storage.contains(index));
            my_storage.construct(index, val);
            REQUIRE(my_storage.contains(index));
            REQUIRE(my_storage[index].i == val);
        }
    }

    SECTION("destroy")
    {
        my_storage.construct(0, 5);
        my_storage.construct(1, 4);
        my_storage.construct(2, 3);

        my_storage.destroy(1);

        REQUIRE(my_storage.contains(0));
        REQUIRE(!my_storage.contains(1));
        REQUIRE(my_storage.contains(2));

        REQUIRE(my_storage[0].i == 5);
        REQUIRE(my_storage[2].i == 3);
    }

    SECTION("iterate")
    {
        my_storage.construct(0, 5);
        my_storage.construct(2, 4);
        my_storage.construct(1, 3);
        my_storage.construct(4, 2);

        auto validate = [](auto id, auto val) {
            switch (id)
            {
            case 0:
                REQUIRE(val == 5);
                break;
            case 1:
                REQUIRE(val == 3);
                break;
            case 2:
                REQUIRE(val == 4);
                break;
            case 4:
                REQUIRE(val == 2);
                break;
            default:
                REQUIRE(false);
                break;
            }
        };

        for (auto it = my_storage.begin(); it != my_storage.end(); ++it)
        {
            auto index = my_storage.index_of(it);
            validate(index, it->i);
        }

        for (auto rit = my_storage.rbegin(); rit != my_storage.rend(); ++rit)
        {
            auto index = my_storage.index_of(rit);
            validate(index, rit->i);
        }
    }
}

TEST_CASE("sparse_vector")
{
    matter::sparse_vector<my_component> vec;

    SECTION("check invalid")
    {
        CHECK(!vec.contains(0));
        CHECK(!vec.contains(1));
    }

    SECTION("insert")
    {
        vec.push_back(0, my_component(0));

        REQUIRE(vec.contains(0));
        CHECK(vec[0].i == 0);

        SECTION("remove iterator")
        {
            auto it = std::begin(vec);
            REQUIRE(it->i == 0);
            REQUIRE(vec.contains(0));

            vec.erase(it);

            REQUIRE(!vec.contains(0));
        }

        SECTION("remove")
        {
            vec.erase(0);
            REQUIRE(!vec.contains(0));
        }

        for (int i = 1; i < 10; ++i)
        {
            vec.emplace_back(i * 10, i * 10);
        }

        SECTION("remove shifing")
        {
            vec.erase(0);
            REQUIRE(!vec.contains(0));

            for (int i = 1; i < 10; ++i)
            {
                REQUIRE(vec[i * 10].i == i * 10);
            }
        }
    }
}
