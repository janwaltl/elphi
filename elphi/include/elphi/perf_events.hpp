/*******************************************************************************
 * @file perf_events.hpp
 * @copyright Copyright 2022 Jan Waltl.
 * @license This file is released under ElPhi project's license, see LICENSE.
 ******************************************************************************/
#pragma once

#include <optional>
#include <span>
#include <variant>

#include <linux/perf_event.h>
#include <unistd.h>

#include <elphi/file_descriptor.hpp>
#include <elphi/utils.hpp>

namespace elphi {

/*******************************************************************************
 * @brief RAII wrapper around perf_event Linux subsystem.
 *
 * Abstracts the ring buffer away, while exposing perf_event types with safe
 * resource management.
 ******************************************************************************/
class PerfEvents {
public:
    /*******************************************************************************
     * @brief Create a new event through perf_event_open syscall.
     *
     * @see `man 2 perf_event_open` for explanation of the parameters.
     *
     * @param attr Config for the event.
     * @param pid Which process to monitor.
     * @param cpu Which CPU to monitor.
     * @param group_fd Group leader for events.
     * @param flags Flags for the event descriptor.
     * @param num_pages Number of pages to allocate for the ring event buffer.
     *  Must be at least 1.
     * @throw ElphiException if the event cannot be initialized.
     ******************************************************************************/
    PerfEvents(const perf_event_attr& attr, pid_t pid, int cpu, int group_fd, std::uint64_t flags,
               std::size_t num_pages);

    /*******************************************************************************
     * @brief Move-only.
     ******************************************************************************/
    PerfEvents(const PerfEvents&) = delete;

    /*******************************************************************************
     * @brief Reclaim @p other resources, left in undetermined state.
     ******************************************************************************/
    PerfEvents(PerfEvents&& other) noexcept;

    /*******************************************************************************
     * @brief Move-only.
     ******************************************************************************/
    PerfEvents&
    operator=(const PerfEvents&) = delete;

    /*******************************************************************************
     * @brief Reclaim @p other resources, left in undetermined state.
     ******************************************************************************/
    PerfEvents&
    operator=(PerfEvents&& other) noexcept;

    /*******************************************************************************
     * @brief Destroy the event descriptor and any uncollected events.
     ******************************************************************************/
    ~PerfEvents() noexcept;

    /*******************************************************************************
     * @brief Pop the first event from the event buffer.
     *
     * @param dest Buffer to store the event, optional. Without perf_event_header.
     * @param peek_only Whether to keep the event in the event buffer or pop it.
     * @return header of the event denoting its type. Note that the size includes
     *  the header itself.
     * @retval Nothing if there is no event.
     ******************************************************************************/
    std::optional<perf_event_header>
    get_perf_event(Buffer* dest, bool peek_only);

    /*******************************************************************************
     * @brief Start/resume the event collection.
     *
     * @param do_reset Reset any stats accumulated so far.
     * @return Whether the event have been (re)started successfully.
     ******************************************************************************/
    [[nodiscard]] bool
    perf_start(bool do_reset = false) noexcept;

    /*******************************************************************************
     * @brief Pause the event collection.
     *
     * Errors are ignored.
     ******************************************************************************/
    void
    perf_stop() noexcept;

    /*******************************************************************************
     * @brief Return the underlying file descriptor.
     ******************************************************************************/
    const FileDescriptor&
    fd() const noexcept;

private:
    /*! Memory-mapped area with the event ring buffer used by perf_event_open. */
    using PerfEventBuffer = std::span<unsigned char>;

    /*******************************************************************************
     * @brief Map the event buffer used by perf_event_open for records.
     *
     * The buffer will have one extra page because the first one is used for the
     * event buffer's header.
     *
     * @param num_pages Size of the buffer to allocate in number of OS pages.
     * @return The allocated buffer, empty and set errno on errors.
     ******************************************************************************/
    [[nodiscard]] PerfEventBuffer
    map_perf_event_buffer(std::size_t num_pages) noexcept;

    /*******************************************************************************
     * @brief Unmap previously mapped event_buffer.
     ******************************************************************************/
    void
    unmap_perf_event_buffer() noexcept;

    /*! Event FD. */
    FileDescriptor m_fd;
    /*! MMapped ring buffer. */
    PerfEventBuffer m_buffer;
};
} // namespace elphi
