#include <cassert>
#include <chrono>
#include <thread>

#include <fmt/format.h>
#include <linux/perf_event.h>
#include <sys/mman.h>
#include <syscall.h>
#include <unistd.h>

#include <elphi/perf_cpu_sampler.hpp>
#include <elphi/perf_events.hpp>
#include <elphi/utils.hpp>

namespace elphi::data {
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

} // namespace

PerfCpuSampler::PerfCpuSampler(CpuId sampled_cpu, std::optional<ProcId> pid, std::size_t frequency) {
    perf_event_attr attr = {};

    int group_fd = -1;
    std::uint64_t flags = PERF_FLAG_FD_CLOEXEC;

    attr.type = PERF_TYPE_SOFTWARE;
    attr.size = sizeof(attr);
    attr.config = PERF_COUNT_SW_CPU_CLOCK;
    attr.sample_freq = frequency;
    attr.freq = 1;

    attr.sample_type = PERF_SAMPLE_TID | PERF_SAMPLE_TIME | PERF_SAMPLE_ADDR | PERF_SAMPLE_CPU;
    attr.read_format = 0;

    attr.disabled = 1;
    attr.sample_id_all = 0;
    // Target 1sec poll roughly.
    attr.wakeup_events = frequency;

    const auto exp_size_per_sec = frequency * (sizeof(perf_event_header) + sizeof(RecordSample));
    //+1 for rounding, ensuring minimal size.
    auto num_pages = (exp_size_per_sec * c_sample_buff_size_secs) / c_page_size + 1;
    num_pages = 1024;

    m_event = FileDescriptor{
        open_perf_event(attr, pid ? static_cast<int>(*pid) : -1, static_cast<int>(sampled_cpu), group_fd, flags)};

    if (!m_event.is_opened()) {
        throw ElphiException(fmt::format("Cannot create PerfSampler: {}", strerror(errno)));
    }

    m_event_buff = map_perf_event_buffer(m_event.raw(), num_pages);

    if (m_event_buff.empty()) {
        throw ElphiException(fmt::format("Cannot map PerfSampler's buffer: {}", strerror(errno)));
    }
}

PerfCpuSampler::PerfCpuSampler(PerfCpuSampler&& other) noexcept :
    m_event(std::move(other.m_event)), m_event_buff(other.m_event_buff),
    m_record_cache(std::move(other.m_record_cache)), m_sampling(other.m_sampling) {}

PerfCpuSampler&
PerfCpuSampler::operator=(PerfCpuSampler&& other) noexcept {
    if (this != &other) {
        this->reset();
        this->m_event = std::move(other.m_event);
        this->m_event_buff = other.m_event_buff;
        this->m_record_cache = std::move(other.m_record_cache);
        this->m_sampling = other.m_sampling;
    }
    return *this;
}

PerfCpuSampler::~PerfCpuSampler() noexcept { reset(); }

void
PerfCpuSampler::reset() noexcept {
    if (is_sampling()) {
        stop();
    }
    unmap_perf_event_buffer(m_event_buff);
}

void
PerfCpuSampler::start() {
    if (!perf_start(m_event.raw()))
        throw ElphiException(fmt::format("Cannot start sampling: {}", strerror(errno)));
    m_sampling = true;
}

bool
PerfCpuSampler::is_sampling() const noexcept {
    return m_sampling;
}

std::optional<Sample>
PerfCpuSampler::get_next() {
    Buffer dest;
    auto maybe_header = get_perf_event(m_event_buff, &dest, false);
    if (maybe_header && maybe_header->type == PERF_RECORD_SAMPLE) {
        assert(dest.size() == sizeof(RecordSample));
        const RecordSample* rec_sample = elphi::type_pune<RecordSample>(dest.data());


        Sample sample{.pid = static_cast<ProcId>(rec_sample->m_pid),
                      .tid = rec_sample->m_tid,
                      .cpu = rec_sample->m_cpu,
                      .time = std::chrono::nanoseconds(rec_sample->m_time)};

        return sample;
    }
    return std::nullopt;
}

pollfd
PerfCpuSampler::get_poll_entry() const noexcept {
    return pollfd{.fd = m_event.raw(), .events = POLLIN, .revents = 0};
}

void
PerfCpuSampler::stop() {
    perf_stop(m_event.raw());
    m_sampling = false;
}

} // namespace elphi::data
