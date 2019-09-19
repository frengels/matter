#include <catch2/catch.hpp>

#include "matter/container/emplace_back.hpp"

#include <tuple>
#include <vector>

// should resolve through either push_back or construct value_type and then
// push_back
template<typename T>
struct only_push_back : private std::vector<T>
{
    using std::vector<T>::vector;

    constexpr auto& base() noexcept
    {
        return static_cast<std::vector<T>&>(*this);
    }

    decltype(auto) begin()
    {
        return base().begin();
    }

    template<typename U>
    decltype(auto) push_back(U&& u)
    {
        return base().push_back(std::forward<U>(u));
    }
};

namespace my
{
struct free_emplacer
{
    friend void emplace_back(free_emplacer&, int)
    {}
};
} // namespace my

TEST_CASE("emplace_back")
{
    SECTION("emplace_back_member")
    {
        auto vec = std::vector<int>{};

        matter::emplace_back(vec, 5);
    }

    SECTION("emplace_back_free")
    {
        auto free = my::free_emplacer{};

        matter::emplace_back(free, 1);
    }

    SECTION("push_back_direct")
    {
        auto pusher = only_push_back<int>{};

        matter::emplace_back(pusher, 50);
    }

    SECTION("push_back_value_t")
    {
        auto pusher = only_push_back<std::tuple<int, int>>{};
        matter::emplace_back(pusher, 5, 5);
    }
}
