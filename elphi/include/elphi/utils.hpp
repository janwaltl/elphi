/*******************************************************************************
 * Various utilities used in ElPhi.
 ******************************************************************************/
#pragma once

#include <algorithm>
#include <array>
#include <cstring>
#include <ranges>
#include <span>
#include <vector>

#include <unistd.h>

namespace elphi {

/*! Generic buffer of bytes. */
using Buffer = std::vector<unsigned char>;

/*******************************************************************************
 * @brief Compiler memory barrier.
 ******************************************************************************/
inline void
read_memory_barrier() {
    asm volatile("" ::: "memory");
}

/*******************************************************************************
 * @brief Semi-safely type pune in C++20.
 *
 * From: https://stackoverflow.com/q/72511754/7691729 .
 *
 * @tparam T Object present at @p ptr.
 * @param ptr Pointer with bytes representing T object.
 * @return @p ptr bytes interpreted as T.
 ******************************************************************************/
template <typename T>
constexpr T*
type_pune(void* ptr) {
    static_assert(std::is_standard_layout_v<T>, "T must be POD.");
    // NOLINTNEXTLINE
    auto* dest = new (ptr) unsigned char[sizeof(T)];
    return reinterpret_cast<T*>(dest);
}

/*******************************************************************************
 * @brief Move elements from circular @p src to fill @p dest.
 *
 * Move `n=min(src.size(),dest.size())` elements from `src[offset:offset+n)`
 * to `dest[0,n)`. Where the range wraps in circular way.
 *
 * @tparam T element type.
 * @param src Source to move from.
 * @param src_offset Offset into @p src from where to start moving elements.
 * @param dest Destination to fill.
 * @return Offset in @p of the first not-moved element.
 *  `src[(offset + n) % src.size()]`.
 ******************************************************************************/
template <std::ranges::sized_range RangeSrc, std::ranges::sized_range RangeDest>
std::size_t
move_wrapped(RangeSrc&& src, std::size_t src_offset, RangeDest&& dest) {
    // Restrict the number of elements to move.
    std::span dest_span{dest.begin(), std::min(dest.size(), src.size())};
    std::span src_span{src};

    // Move from offset possibly to the end.
    auto first = src_span.subspan(src_offset);
    auto first_n = std::min(first.size(), dest_span.size());
    auto second_dest = std::move(first.begin(), first.begin() + first_n, dest_span.begin());
    // Move any left-over wrapped portion.
    auto second = src_span.first(dest_span.size() - first_n);
    std::move(second.begin(), second.end(), second_dest);

    return (src_offset + dest_span.size()) % std::max(src_span.size(), 1UL);
}

/*! OS Page size of the running system. */
inline const std::size_t c_page_size = getpagesize();

/*******************************************************************************
 * @brief C++ version of strerror, thread-safe.
 *
 * @param err Error code (errno).
 ******************************************************************************/
std::string
strerror(int err);
} // namespace elphi
