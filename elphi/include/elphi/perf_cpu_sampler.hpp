#pragma once

#include <poll.h>

#include <elphi/file_descriptor.hpp>
#include <elphi/perf_events.hpp>
#include <elphi/sampler.hpp>

namespace elphi::data {

/*******************************************************************************
 * @brief Sample CPU activity.
 *
 * Not thread-safe.
 ******************************************************************************/
class PerfCpuSampler {
public:
    /*******************************************************************************
     * @brief Configure a new sampler
     *
     * @param sampled_cpu CPU to sample.
     * @param pid Optionally restrict the sampling to given process ID.
     * @param frequence Sampling frequency. Number of samples per second to take.
     * @throws ElphiException if the new sampler cannot be created/configured.
     ******************************************************************************/
    PerfCpuSampler(CpuId sampled_cpu, std::optional<ProcId> pid, std::size_t frequency);

    /*******************************************************************************
     * @brief Move-only, samplers cannot be copied.
     ******************************************************************************/
    PerfCpuSampler(const PerfCpuSampler&) = delete;

    /*******************************************************************************
     * @brief Reclaim resources from @p other .
     *
     * @p other is left in unusable state and must be destroyed.
     ******************************************************************************/
    PerfCpuSampler(PerfCpuSampler&& other) noexcept;

    /*******************************************************************************
     * @brief Move-only, samplers cannot be copied.
     ******************************************************************************/
    PerfCpuSampler&
    operator=(const PerfCpuSampler&) = delete;

    /*******************************************************************************
     * @brief Stop the sampling before capturing resources from @p other.
     *
     * @p other is left in unusable state and must be destroyed.
     ******************************************************************************/
    PerfCpuSampler&
    operator=(PerfCpuSampler&& other) noexcept;

    /*******************************************************************************
     * @brief Stop any running sampling, destroy the sampler.
     *
     * Unextracted samples are lost.
     ******************************************************************************/
    ~PerfCpuSampler() noexcept;

    /*******************************************************************************
     * @brief Start collecting performance samples.
     *
     * @note The user must call get_next() periodically to retrieve the samples.
     * @throws ElphiException if the sampling failed to start.
     ******************************************************************************/
    void
    start();

    /*******************************************************************************
     * @brief Whether we are sampling right now.
     *
     * Safe to call for moved-from instances, returns false.
     ******************************************************************************/
    bool
    is_sampling() const noexcept;

    /*******************************************************************************
     * @brief Retrieve the next sample, non-blocking.
     *
     * @see get_poll_entry() to poll until samples are generated.
     *
     * @warn Samples are gathered by perf_event which uses ring buffer, therefore
     * there is hard limit on number of pending samples unextracted samples.
     * One must call get_next() often enough to not lose any.
     *
     * @return Sample
     * @retval Nothing if not called within start(), end() or there is no sample.
     ******************************************************************************/
    std::optional<Sample>
    get_next();

    /*******************************************************************************
     * @brief Poll for incoming samples.
     *
     * @note Currently, the implementation targets to trigger this roughly every second
     *  with `frequency` number of samples waiting.
     *
     * @return Filled-out poll entry for this sampler. When POLLIN is received,
     *  call get_next() to retrieve the new samples.
     ******************************************************************************/
    pollfd
    get_poll_entry() const noexcept;

    /*******************************************************************************
     * @brief End collection of samples.
     *
     * @note One should keep calling get_next() after this call until no sample
     * is returned.
     *
     * @remark If sampling did not start, this is no-op.
     ******************************************************************************/
    void
    stop();

private:
    /*******************************************************************************
     * @brief Reset internal state, release all resources.
     ******************************************************************************/
    void
    reset() noexcept;

    /*! File descriptor for the event. */
    FileDescriptor m_event;
    /*! Ring buffer with stored event samples. */
    PerfEventBuffer m_event_buff;
    /*! Buffer used for extracting records, memory cached throughout calls. */
    Buffer m_record_cache;
    /*! Whether sampling is enabled or not. */
    bool m_sampling;
};
} // namespace elphi::data
