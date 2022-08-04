/*******************************************************************************
 * @file cpu_sampler.cpp
 * @copyright Copyright 2022 Jan Waltl.
 * @license	This file is released under ElPhi project's license, see LICENSE.
 ******************************************************************************/
#include <algorithm>
#include <cassert>

#include <fmt/format.h>
#include <sys/poll.h>

#include <elphi/cpu_sampler.hpp>
#include <elphi/perf_events.hpp>

namespace elphi {

namespace {

/*******************************************************************************
 * @brief Recorded perf_event sample.
 *
 * Layout matches the expected struct in the ring buffer.
 ******************************************************************************/
struct RecordSample {
    std::uint32_t m_pid;
    std::uint32_t m_tid;
    std::uint64_t m_time;
    std::uint64_t m_addr;
    std::uint32_t m_cpu;
    std::uint32_t m_cpu_pad;
};
static_assert(sizeof(RecordSample) == 32, "The sample must match the record in the ring buffer.");

/*! Use buffer large enough to store ten seconds worth of samples. */
constexpr const std::size_t c_sample_buff_size_secs = 10;
/*! Wakeup targets 1s, +1s for good measure -> no timeout hopefully. */
constexpr const std::size_t c_poll_timeout_ms = 2000;

[[nodiscard]] perf_event_attr
creat_attribs(std::size_t frequency) noexcept {
    perf_event_attr attr = {};


    attr.type = PERF_TYPE_SOFTWARE;
    attr.size = sizeof(attr);
    attr.config = PERF_COUNT_SW_TASK_CLOCK;
    attr.sample_freq = frequency;
    attr.freq = 1;

    attr.sample_type = PERF_SAMPLE_TID | PERF_SAMPLE_TIME | PERF_SAMPLE_ADDR | PERF_SAMPLE_CPU;
    attr.read_format = 0;

    attr.disabled = 1;
    attr.sample_id_all = 0;
    // Target 1sec poll roughly.
    attr.wakeup_events = frequency;
    return attr;
}
} // namespace


CpuSamplingResult
sample_cpus_sync(const std::vector<CpuId>& cpus, std::size_t sample_frequency, const std::stop_token& token) {
    std::uint64_t flags = PERF_FLAG_FD_CLOEXEC;

    const auto exp_size_per_sec = sample_frequency * (sizeof(perf_event_header) + sizeof(RecordSample));
    //+1 for rounding, ensuring minimal size.
    auto num_pages = (exp_size_per_sec * c_sample_buff_size_secs) / c_page_size + 1;

    auto attribs = creat_attribs(sample_frequency);


    std::vector<PerfEvents> events;
    std::vector<pollfd> entries;
    for (auto cpu_id : cpus) {
        events.emplace_back(attribs, -1, static_cast<int>(cpu_id), -1, flags, num_pages);
        entries.push_back({.fd = events.back().fd().raw(), .events = POLLIN, .revents = 0});
    }

    for (auto& event : events)
        if (!event.perf_start(true))
            throw ElphiException("Cannot start the sampling events.");

    CpuSamplingResult result;

    while (token.stop_possible() && !token.stop_requested()) {
        auto num_active = poll(entries.data(), entries.size(), c_poll_timeout_ms);
        if (num_active == -1)
            throw ElphiException(fmt::format("Polling failure: ", strerror(errno)));

        Buffer dest;
        for (auto& event : events)
            while (auto maybe_header = event.get_perf_event(&dest, false)) {
                if (maybe_header->type != PERF_RECORD_SAMPLE)
                    continue;

                assert(dest.size() == sizeof(RecordSample));
                const RecordSample* rec_sample = elphi::type_pune<RecordSample>(dest.data());
                CpuSample sample{.pid = static_cast<ProcId>(rec_sample->m_pid),
                                 .tid = rec_sample->m_tid,
                                 .cpu = rec_sample->m_cpu,
                                 .time = std::chrono::nanoseconds(rec_sample->m_time)};
                result.samples.push_back(sample);
            }

        for (auto& entry : entries)
            entry.revents = 0;
    }

    return result;
}
} // namespace elphi
