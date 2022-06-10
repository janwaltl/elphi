#include <cassert>
#include <cerrno>
#include <cstdint>
#include <cstdio>
#include <cstring>
#include <ctime>

#include <linux/perf_event.h>
#include <sys/ioctl.h>
#include <sys/mman.h>
#include <syscall.h>
#include <unistd.h>

#include <elphi/perf_events.hpp>

#include "elphi/utils.hpp"
#include "fmt/core.h"

namespace elphi::data {
namespace {
const std::size_t c_page_size = getpagesize();
} // namespace

int // NOLINTNEXTLINE - unsigned long on purpose.
open_perf_event(const perf_event_attr& attr, pid_t pid, int cpu, int group_fd, unsigned long flags) noexcept {
    // NOLINTNEXTLINE - syscall is vararg.
    auto res = syscall(SYS_perf_event_open, &attr, pid, cpu, group_fd, flags);
    return static_cast<int>(res);
}

PerfEventBuffer
map_perf_event_buffer(int event_fd, std::size_t num_pages) noexcept {
    if (event_fd < 0 || num_pages == 0)
        return {};

    //+1 for the buffer header.
    std::size_t map_size = c_page_size * (1 + num_pages);
    auto* ptr = static_cast<unsigned char*>(mmap(nullptr, map_size, PROT_READ | PROT_WRITE, MAP_SHARED, event_fd, 0));

    return ptr ? PerfEventBuffer{ptr, map_size} : PerfEventBuffer{};
}

void
unmap_perf_event_buffer(PerfEventBuffer event_buffer) noexcept {
    if (event_buffer.empty() && (event_buffer.size() % c_page_size) == 0)
        (void)munmap(event_buffer.data(), event_buffer.size());
}


std::optional<perf_event_header>
get_perf_event(PerfEventBuffer event_buffer, Buffer* dest, bool peek_only) {
    if (event_buffer.empty() || (event_buffer.size() % c_page_size) != 0)
        return std::nullopt;

    auto* head = type_pune<perf_event_mmap_page>(event_buffer.data());
    const auto buffer = event_buffer.subspan(c_page_size);

    // Extract header from the buffer.
    perf_event_header event_header;
    // Header does not fit -> no unread sample.
    if (head->data_tail + sizeof(event_header) > head->data_head)
        return std::nullopt;
    read_memory_barrier();

    std::span header_dest{reinterpret_cast<unsigned char*>(&event_header), sizeof(event_header)};
    move_wrapped(buffer, head->data_tail, header_dest);

    if (dest) {
        // Whole event has not been fully written yet.
        // Can it even happen?
        if (head->data_tail + event_header.size > head->data_head)
            return std::nullopt;
        dest->resize(event_header.size - sizeof(perf_event_header));
        // Some examples and manpages recommend memory barries after reading the values, not sure why.
        read_memory_barrier();

        move_wrapped(buffer, head->data_tail + sizeof(event_header), *dest);
    }

    if (!peek_only) {
        head->data_tail += event_header.size;
        read_memory_barrier();
    }

    return event_header;
}

bool
perf_start(int event_fd, bool do_reset) noexcept {
    return (do_reset && ioctl(event_fd, PERF_EVENT_IOC_RESET, 0) == 0) ||
           ioctl(event_fd, PERF_EVENT_IOC_ENABLE, 0) == 0;
}

void
perf_stop(int event_fd) noexcept {
    ioctl(event_fd, PERF_EVENT_IOC_DISABLE, 0);
}

} // namespace elphi::data
