#include <algorithm>
#include <ranges>
#include <span>

#include <catch2/catch_all.hpp>
#include <catch2/matchers/catch_matchers_all.hpp>
#include <elphi/utils.hpp>

#include "utils.hpp"


using vec = std::vector<int>;

TEST_CASE("Wrapped move from empty range") {
    constexpr std::size_t offset = 0;

    SECTION("to empty range does nothing.") {
        auto next_src = elphi::move_wrapped(std::span<int>{}, offset, std::span<int>{});
        // Sanitizers should catch the errors.
        REQUIRE(next_src == offset);
    }

    SECTION("to non-empty range does not modify dest.") {
        vec dest{1, 2, 3, 4, 5, 6};
        const auto exp_dest = dest;
        auto next_src = elphi::move_wrapped(std::span<int>{}, offset, dest);

        REQUIRE(dest == exp_dest);
        REQUIRE(next_src == offset);
    }
}

TEST_CASE("Wrapped move to empty range") {
    constexpr std::size_t offset = 0;

    SECTION("from empty range does nothing.") {
        auto next_src = elphi::move_wrapped(std::span<int>{}, offset, std::span<int>{});
        // Sanitizers should catch the errors.
        REQUIRE(next_src == offset); // Nothing moved.
    }

    SECTION("from non-empty range moves no elements.") {
        vec src{1, 2, 3, 4, 5, 6};
        const auto exp_src = src;
        auto next_src = elphi::move_wrapped(src, offset, std::span<int>{});

        REQUIRE(exp_src == src);
        REQUIRE(next_src == offset); // Nothing moved.
    }
}

TEST_CASE("Wrapped move acts rotation if sizes match.") {
    const size_t offset = GENERATE(0, 1, 2, 3, 4);
    auto src = vec{1, 2, 3, 4, 5, 6};
    auto dst = vec{0, 0, 0, 0, 0, 0};
    auto exp = src;

    std::rotate(exp.begin(), exp.begin() + static_cast<ssize_t>(offset), exp.end());

    auto next_src = elphi::move_wrapped(src, offset, dst);

    REQUIRE_THAT(dst, EqualsRange(exp));
    REQUIRE(next_src == offset);
}

TEST_CASE("Wrapped move with wrapping") {

    size_t offset{};
    vec src;
    vec dst;
    vec exp_dst;
    SECTION("#1") {
        offset = 1;
        src = vec{1, 2, 3};
        dst = vec{0, 0, 0, 0, 0, 0};
        exp_dst = vec{2, 3, 1, 0, 0, 0};
    }

    SECTION("#2") {
        offset = 2;
        src = vec{1, 2, 3};
        dst = vec{0, 0, 0, 0, 0, 0};
        exp_dst = vec{3, 2, 1, 0, 0, 0};
    }

    SECTION("#3") {
        offset = 7;
        src = vec{1, 2, 3, 4, 5, 6, 7, 8};
        dst = vec{0, 0, 0, 0, 0, 0};
        exp_dst = vec{8, 1, 2, 3, 4, 5};
    }
    auto next_src = elphi::move_wrapped(src, offset, dst);

    REQUIRE_THAT(dst, EqualsRange(exp));
    const auto mod = std::max(1UL, src.size());
    const auto exp_next = (offset + std::min(src.size(), dst.size())) % mod;
    REQUIRE(next_src == exp_next);
}
