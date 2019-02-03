#include <catch2/catch.hpp>

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

    using straits = matter::storage_traits<decltype(my_storage)>;

    SECTION("emplace")
    {
        auto index = GENERATE(as<uint32_t>(), 0, 1, 3, 5, 9, 100, 80);

        SECTION("values")
        {
            auto val = GENERATE(5, 4, 3, 9, 10, 300, 9001);

            REQUIRE(!straits::contains(my_storage, index));
            straits::emplace(my_storage, index, val);
            REQUIRE(straits::contains(my_storage, index));
            REQUIRE(straits::get(my_storage, index).i == val);
        }
    }

    SECTION("erase")
    {
        straits::emplace(my_storage, 0, 5);
        straits::emplace(my_storage, 1, 4);
        straits::emplace(my_storage, 2, 3);

        straits::erase(my_storage, 1);

        REQUIRE(straits::contains(my_storage, 0));
        REQUIRE(!straits::contains(my_storage, 1));
        REQUIRE(straits::contains(my_storage, 2));

        REQUIRE(straits::get(my_storage, 0).i == 5);
        REQUIRE(straits::get(my_storage, 2).i == 3);
    }

    SECTION("iterate")
    {
        straits::emplace(my_storage, 0, 5);
        straits::emplace(my_storage, 2, 4);
        straits::emplace(my_storage, 1, 3);
        straits::emplace(my_storage, 4, 2);

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

        for (auto it = straits::begin(my_storage);
             it != straits::end(my_storage);
             ++it)
        {
            auto index = straits::index_of(my_storage, it);
            validate(index, it->i);
        }

        for (auto rit = straits::rbegin(my_storage);
             rit != straits::rend(my_storage);
             ++rit)
        {
            auto index = straits::index_of(my_storage, rit);
            validate(index, rit->i);
        }
    }
}
