/*******************************************************************************
 * @file perf_events.hpp
 * Thin C-like wrapper around perf_event_open syscall.
 ******************************************************************************/
#pragma once

#include <optional>
#include <span>
#include <variant>

#include <linux/perf_event.h>
#include <unistd.h>

#include <elphi/utils.hpp>

namespace elphi::data {

/*! Memory-mapped area with the event ring buffer used by perf_event_open. */
using PerfEventBuffer = std::span<unsigned char>;

/*******************************************************************************
 * @brief perf_event_open syscall.
 *
 * @see `man perf_event_open`.
 *
 * The descriptor must be closed via close() syscall.
 *
 * @param attr Config for the event.
 * @param pid Which process to trace.
 * @param cpu Which CPU to trace.
 * @param group_fd Group leader for events.
 * @param flags Flags for the event descriptor.
 * @return FD of the event on success, -1 and errno on error.
 ******************************************************************************/
int
open_perf_event(const perf_event_attr& attr, pid_t pid, int cpu, int group_fd, unsigned long flags) noexcept;

/*******************************************************************************
 * @brief Map the event buffer used by perf_event_open for records.
 *
 * The buffer will have one extra page because the first one is used for the
 * event buffer's header.
 *
 * @param event_fd Filedescriptor from open_perf_event().
 * @param num_pages Size of the buffer to allocate in number of OS pages.
 * @return The allocated buffer, empty and errno on errors.
 ******************************************************************************/
PerfEventBuffer
map_perf_event_buffer(int event_fd, std::size_t num_pages) noexcept;

/*******************************************************************************
 * @brief Unmap previously mapped event_buffer.
 *
 * @param event_buffer Result of map_perf_event_buffer() call.
 ******************************************************************************/
void
unmap_perf_event_buffer(PerfEventBuffer event_buffer) noexcept;

/*******************************************************************************
 * @brief Pop the first event from the event buffer.
 *
 * @param event_buffer Result of map_perf_event_buffer() call.
 * @param dest Buffer to store the event, optional. Without perf_event_header.
 * @param peek_only Whether to keep the event in the event buffer or pop it.
 * @return header of the event denoting its type. Note that the size includes
 *  the header itself.
 * @retval Nothing if there is no event.
 ******************************************************************************/
std::optional<perf_event_header>
get_perf_event(PerfEventBuffer event_buffer, Buffer* dest, bool peek_only);

/*******************************************************************************
 * @brief Start/resume the event.
 *
 * @param event_fd Filedescriptor from open_perf_event().
 * @param do_reset Reset any stats accumulated so far.
 * @return Whether the event have been (re)started successfully.
 ******************************************************************************/
bool
perf_start(int event_fd, bool do_reset = false) noexcept;

/*******************************************************************************
 * @brief Pause the event.
 *
 * Errors are ignored.
 *
 * @param event_fd Opened event.
 ******************************************************************************/
void
perf_stop(int event_fd) noexcept;
} // namespace elphi::data
