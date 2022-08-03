/*******************************************************************************
 * Unit test perf_events.
 * DISABLED for now
 ******************************************************************************/
#if false

#include <linux/perf_event.h>
#include <unistd.h>

#include <catch2/catch_test_macros.hpp>
#include <catch2/generators/catch_generators.hpp>
#include <elphi/perf_events.hpp>
#include <elphi/utils.hpp>

#include "mock_syscalls.hpp"


TEST_CASE("Zero pages events") {
    SysMock::set_mmap_clbk([](const auto&...) {
        FAIL_CHECK("Must not be called for zero pages.");
        return nullptr; // Return an error.
    });

    REQUIRE(buffer.empty());
}

SECTION("zero pages") {

    int exp_fd = 3;
    size_t num_pages = 0;
    auto buffer = elphi::data::map_perf_event_buffer(exp_fd, num_pages);

    REQUIRE(buffer.empty());
}
}

TEST_CASE("map perf_events calls correct mmap") {
    int exp_fd = 5;
    size_t num_pages = GENERATE(1, 2, 4, 8);


    SysMock::set_mmap_clbk([exp_fd, num_pages](void* addr, size_t len, int prot, int flags, int fd, off_t offset) {
        CHECK(len == (c_page_size * (num_pages + 1)));
        CHECK(prot == (PROT_READ | PROT_WRITE));
        CHECK(flags == MAP_SHARED);
        CHECK(offset == 0);
        CHECK(addr == nullptr);
        CHECK(exp_fd == fd);
        return reinterpret_cast<void*>(-1); // Still return error.
    });

    auto buffer = elphi::data::map_perf_event_buffer(exp_fd, num_pages);

    REQUIRE(buffer.empty());
}

TEST_CASE("map perf_events success") {
    int exp_fd = 5;
    size_t num_pages = 3;

    elphi::Buffer vec;
    vec.resize((c_page_size * (num_pages + 1)));


    SysMock::set_mmap_clbk([&vec](void*, size_t, int, int, int, off_t) { return vec.data(); });

    auto buffer = elphi::data::map_perf_event_buffer(exp_fd, num_pages);

    // Must pass the span returned from map.
    const elphi::data::PerfEventBuffer exp_span{vec};
    REQUIRE(buffer.size() == exp_span.size());
    REQUIRE(buffer.data() == exp_span.data());
}

TEST_CASE("unmap perf_events") {
    SECTION("empty buffer is noop.") {
        SysMock::set_munmap_clbk([](const auto&...) {
            FAIL_CHECK("munmap must not be called.");
            return -1;
        });
        elphi::data::unmap_perf_event_buffer(elphi::data::PerfEventBuffer{});
    }

    SECTION("passing not page-aligned buffer is noop.") {
        elphi::Buffer buffer;
        buffer.resize(c_page_size + 1);
        SysMock::set_munmap_clbk([](const auto&...) {
            FAIL_CHECK("munmap must not be called.");
            return -1;
        });
        elphi::data::unmap_perf_event_buffer(elphi::data::PerfEventBuffer{buffer});
    }

    SECTION("correct munmap is called.") {
        elphi::Buffer buffer;
        buffer.resize(c_page_size * 5);
        SysMock::set_munmap_clbk([&buffer](void* addr, std::size_t len) {
            CHECK(len == buffer.size());
            CHECK(buffer.data() == addr);
            return 0;
        });
        elphi::data::unmap_perf_event_buffer(elphi::data::PerfEventBuffer{buffer});
    }
}

TEST_CASE("Get next perf_event empty buffer contains no events") {
    elphi::Buffer buffer;
    buffer.resize(c_page_size * 5);

    elphi::data::get_perf_event(elphi::data::PerfEventBuffer{}, &buffer, false);
}

SCENARIO("Getting the next perf event (single)") {
    elphi::Buffer storage;
    storage.resize(c_page_size * 5);

    elphi::data::PerfEventBuffer buffer{storage};
    elphi::Buffer dest;
    perf_event_mmap_page page{};

    GIVEN("Empty ring buffer") {
        // Empty ring buffer with different offsets.
        page.data_head = GENERATE_REF(0, 4, 14, storage.size() - c_page_size - 1);
        page.data_tail = page.data_head;

        REQUIRE(storage.size() >= sizeof(page));
        std::memcpy(storage.data(), &page, sizeof(page));

        WHEN("Getting an event with destination") {
            auto maybe_event = elphi::data::get_perf_event(buffer, &dest, GENERATE(true, false));
            THEN("No event is extracted") {
                REQUIRE(!maybe_event.has_value());
                REQUIRE(dest.empty());
            }
        }
        WHEN("Getting an event without destination") {
            auto maybe_event = elphi::data::get_perf_event(buffer, nullptr, GENERATE(true, false));
            THEN("No event is extracted") { REQUIRE(!maybe_event.has_value()); }
        }
    }

    GIVEN("Buffer with a single event") {
        elphi::Buffer exp_event = GENERATE(elphi::Buffer{1, 2, 3, 4, 5, 6}, elphi::Buffer{});
        const perf_event_header header{
            .type = 12, .misc = 241, .size = static_cast<std::uint16_t>(sizeof(header) + exp_event.size())};
        page.data_tail = 14;
        // Event with no payload
        page.data_head = page.data_tail + header.size;

        std::memcpy(storage.data(), &page, sizeof(page));
        // RingBuffer starts at the second page, at tail.
        std::memcpy(storage.data() + c_page_size + page.data_tail, &header, sizeof(header));
        std::copy(exp_event.begin(), exp_event.end(), storage.data() + c_page_size + page.data_tail + sizeof(header));

        WHEN("Popping an event") {
            auto maybe_event = elphi::data::get_perf_event(buffer, &dest, false);
            THEN("The event is extracted from the ring buffer") {
                REQUIRE(maybe_event.has_value());
                REQUIRE(maybe_event->size == header.size);
                REQUIRE(maybe_event->type == header.type);
                REQUIRE(maybe_event->misc == header.misc);
                REQUIRE(exp_event == dest);
                // Tail is incremented correctly.
                std::memcpy(&page, storage.data(), sizeof(page));
                REQUIRE(page.data_tail == page.data_head);
            }
            THEN("There is no second event") {
                auto maybe_second = elphi::data::get_perf_event(buffer, &dest, false);
                REQUIRE(!maybe_second.has_value());
            }
        }
        WHEN("Peeking an event") {
            auto maybe_event = elphi::data::get_perf_event(buffer, &dest, true);
            THEN("The event is read from the ring buffer") {
                REQUIRE(maybe_event.has_value());
                REQUIRE(maybe_event->size == header.size);
                REQUIRE(maybe_event->type == header.type);
                REQUIRE(maybe_event->misc == header.misc);
                REQUIRE(exp_event == dest);
                // Tail must stay the same.
                auto exp_tail = page.data_tail;
                std::memcpy(&page, storage.data(), sizeof(page));
                REQUIRE(page.data_tail == exp_tail);
            }
            THEN("The second peek returns the same event") {
                auto maybe_second = elphi::data::get_perf_event(buffer, &dest, true);
                REQUIRE(maybe_second.has_value());
                REQUIRE(maybe_second->size == maybe_event->size);
                REQUIRE(maybe_second->type == maybe_event->type);
                REQUIRE(maybe_second->misc == maybe_event->misc);
                REQUIRE(exp_event == dest);
            }
            THEN("The second pop returns the event") {
                auto maybe_second = elphi::data::get_perf_event(buffer, &dest, false);
                REQUIRE(maybe_second.has_value());
                REQUIRE(maybe_second->size == maybe_event->size);
                REQUIRE(maybe_second->type == maybe_event->type);
                REQUIRE(maybe_second->misc == maybe_event->misc);
                REQUIRE(exp_event == dest);
                // Tail is incremented correctly.
                std::memcpy(&page, storage.data(), sizeof(page));
                REQUIRE(page.data_tail == page.data_head);
            }
        }
    }
}

TEST_CASE("Pop multiple perf events") {
    elphi::Buffer storage;
    storage.resize(c_page_size * 5);

    elphi::data::PerfEventBuffer buffer{storage};
    perf_event_mmap_page page{};

    std::array exp_events{GENERATE(elphi::Buffer{1, 2, 3}, elphi::Buffer{}),
                          GENERATE(elphi::Buffer{4, 5, 6}, elphi::Buffer{})};
    std::array headers{
        perf_event_header{.type = 12,
                          .misc = 241,
                          .size = static_cast<std::uint16_t>(sizeof(perf_event_header) + exp_events[0].size())},
        perf_event_header{.type = 12,
                          .misc = 241,
                          .size = static_cast<std::uint16_t>(sizeof(perf_event_header) + exp_events[1].size())}};

    page.data_tail = 14;
    // Event with no payload
    page.data_head = page.data_tail + headers[0].size + headers[1].size;

    std::size_t write_offset = 0;
    std::memcpy(storage.data() + write_offset, &page, sizeof(page));
    write_offset += c_page_size + page.data_tail;

    // Prepare ring buffer.
    for (std::size_t i = 0; i < headers.size(); ++i) {
        const auto& h = headers[i];
        const auto& e = exp_events[i];
        std::memcpy(storage.data() + write_offset, &h, sizeof(h));
        write_offset += sizeof(h);
        std::copy(e.begin(), e.end(), storage.data() + write_offset);
        write_offset += e.size();
    }

    SECTION("Pop+Pop+Pop->Ok,Ok,Nothing") {
        for (std::size_t i{0}; i < headers.size(); ++i) {
            const auto& h = headers[i];
            const auto& e = exp_events[i];

            elphi::Buffer dest;
            auto maybe_event = elphi::data::get_perf_event(buffer, &dest, false);

            REQUIRE(maybe_event.has_value());
            REQUIRE(maybe_event->size == h.size);
            REQUIRE(maybe_event->type == h.type);
            REQUIRE(maybe_event->misc == h.misc);
            REQUIRE(e == dest);
            auto exp_tail = page.data_tail + h.size;
            // Tail is incremented correctly.
            std::memcpy(&page, storage.data(), sizeof(page));
            REQUIRE(page.data_tail == exp_tail);
        }
        // No third event
        elphi::Buffer dest;
        auto maybe_event = elphi::data::get_perf_event(buffer, &dest, false);
        REQUIRE(!maybe_event.has_value());
    }
}
#endif
