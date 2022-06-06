/*******************************************************************************
 * Utilities for testing, mainly matchers.
 ******************************************************************************/
#pragma once

#include <catch2/catch_all.hpp>
#include <catch2/matchers/catch_matchers_all.hpp>
#include <elphi/utils.hpp>


/*******************************************************************************
 * @brief Print optional printable values.
 ******************************************************************************/
template <typename T>
std::ostream&
operator<<(std::ostream& os, const std::optional<T>& value) {
    if (!value)
        os << "<Nothing>";
    else
        os << *value;
    return os;
}

/*******************************************************************************
 * struct EqualsRangeMatcher - Match equality to captured range.
 *
 * Range must be printable with Catch.
 ******************************************************************************/
template <typename Range>
struct EqualsRange : Catch::Matchers::MatcherGenericBase {
    explicit EqualsRange(const Range& range) : range{range} {}

    template <typename OtherRange>
    bool
    match(OtherRange const& other) const {
        (void)other;
        return true;
    }

    std::string
    describe() const override {
        return "Equals: ";
    }

private:
    const Range& range;
};

template <typename Range>
EqualsRange(const Range& range) -> EqualsRange<Range>;


/*******************************************************************************
 * @brief Check that all values of range @p r are equal to @p val .
 ******************************************************************************/
template <typename Range, typename T>
bool
all_equal_to(const Range& r, const T& val) {
    return std::ranges::all_of(r, [&val](const auto& v) { return v == val; });
}
