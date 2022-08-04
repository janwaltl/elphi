/*******************************************************************************
 * @file perf_events.cpp
 * @copyright Copyright 2022 Jan Waltl.
 * @license	This file is released under ElPhi project's license, see LICENSE.
 ******************************************************************************/
#include <cassert>
#include <cerrno>
#include <cstdint>
#include <cstdio>
#include <cstring>
#include <ctime>

#include <fmt/format.h>
#include <linux/perf_event.h>
#include <sys/ioctl.h>
#include <sys/mman.h>
#include <syscall.h>
#include <unistd.h>

#include <elphi/perf_events.hpp>
#include <elphi/utils.hpp>

#include "elphi/exception.hpp"

namespace elphi {

namespace {

extern "C" int // NOLINTNEXTLINE - unsigned long on purpose.
open_perf_event(const perf_event_attr& attr, pid_t pid, int cpu, int group_fd, unsigned long flags) noexcept {
    // NOLINTNEXTLINE - syscall is vararg.
    auto res = syscall(SYS_perf_event_open, &attr, pid, cpu, group_fd, flags);
    return static_cast<int>(res);
}
} // namespace

PerfEvents::PerfEventBuffer
PerfEvents::map_perf_event_buffer(std::size_t num_pages) noexcept {
    if (!m_fd.is_opened() || num_pages == 0)
        return {};

    //+1 for the buffer header.
    std::size_t map_size = c_page_size * (1 + num_pages);
    auto* ptr = static_cast<unsigned char*>(mmap(nullptr, map_size, PROT_READ | PROT_WRITE, MAP_SHARED, m_fd.raw(), 0));

    return reinterpret_cast<std::intptr_t>(ptr) != -1 ? PerfEventBuffer{ptr, map_size} : PerfEventBuffer{};
}

void
PerfEvents::unmap_perf_event_buffer() noexcept {
    if (!m_buffer.empty() && (m_buffer.size() % c_page_size) == 0)
        (void)munmap(m_buffer.data(), m_buffer.size());
    m_buffer = {};
}


std::optional<perf_event_header>
PerfEvents::get_perf_event(Buffer* dest, bool peek_only) {
    if (!m_fd.is_opened() || m_buffer.empty() || (m_buffer.size() % c_page_size) != 0)
        return std::nullopt;

    auto* header = type_pune<perf_event_mmap_page>(m_buffer.data());
    // The ring buffer begins at the next page.
    const auto buffer = m_buffer.subspan(c_page_size);

    perf_event_header event_header;
    // Header does not fit -> no unread sample.
    // Both values are non-decreasing.
    if (header->data_tail + sizeof(event_header) > header->data_head)
        return std::nullopt;
    read_memory_barrier();

    std::span header_dest{reinterpret_cast<unsigned char*>(&event_header), sizeof(event_header)};
    move_wrapped(buffer, header->data_tail % buffer.size(), header_dest);

    if (dest) {
        // The event is only partially written. Can it even happen?
        if (header->data_tail + event_header.size > header->data_head) [[unlikely]]
            return std::nullopt;

        dest->resize(event_header.size - sizeof(perf_event_header));
        // Some examples and manpages recommend memory barries after reading the values, not sure why.
        read_memory_barrier();

        move_wrapped(buffer, (header->data_tail + sizeof(event_header)) % buffer.size(), *dest);
    }

    if (!peek_only) {
        header->data_tail += event_header.size;
        read_memory_barrier();
    }

    return event_header;
}

bool
PerfEvents::perf_start(bool do_reset) noexcept {
    return (do_reset && ioctl(m_fd.raw(), PERF_EVENT_IOC_RESET, 0) == 0) &&
           ioctl(m_fd.raw(), PERF_EVENT_IOC_ENABLE, 0) == 0;
}

void
PerfEvents::perf_stop() noexcept {
    (void)ioctl(m_fd.raw(), PERF_EVENT_IOC_DISABLE, 0);
}

const FileDescriptor&
PerfEvents::fd() const noexcept {
    return m_fd;
}

PerfEvents::PerfEvents(const perf_event_attr& attr, pid_t pid, int cpu, int group_fd, std::uint64_t flags,
                       std::size_t num_pages) {
    m_fd = FileDescriptor{open_perf_event(attr, pid, cpu, group_fd, flags)};
    if (m_fd.raw() == -1)
        throw ElphiException(fmt::format("Failed to open the event, reason: {}", strerror(errno)));

    m_buffer = map_perf_event_buffer(num_pages);
    if (m_buffer.empty())
        throw ElphiException(fmt::format("Failed to map the buffer, reason: {}", strerror(errno)));
}
PerfEvents::PerfEvents(PerfEvents&& other) noexcept : m_fd(std::move(other.m_fd)), m_buffer(other.m_buffer) {
    other.m_buffer = {};
}

PerfEvents&
PerfEvents::operator=(PerfEvents&& other) noexcept {
    if (this != &other) {
        this->unmap_perf_event_buffer();
        this->m_fd = std::move(other.m_fd);
        this->m_buffer = other.m_buffer;
        other.m_buffer = {};
    }
    return *this;
}

PerfEvents::~PerfEvents() noexcept { unmap_perf_event_buffer(); }

} // namespace elphi
