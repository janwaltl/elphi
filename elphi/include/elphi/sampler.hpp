#pragma once

#include <chrono>
#include <cstdint>
#include <stop_token>
#include <string>
#include <unordered_map>
#include <vector>

#include <elphi/exception.hpp>

namespace elphi {

/*! Process ID. */
using ProcId = std::uint32_t;
/*! Thread ID. */
using ThreadId = std::uint32_t;
/*! CPU ID. */
using CpuId = std::uint64_t;
/*! CGroup ID. */
using GroupId = std::uint64_t;
/*! Measure time in nanoseconds.  */
using TimePoint = std::chrono::nanoseconds;

/*******************************************************************************
 * struct Sample - Execution context at sampled point of time.
 *
 * Captures what a given CPU was executing at the sampled time.
 ******************************************************************************/
struct Sample {
    /*! Process ID */
    ProcId pid = 0;
    /*! Thread ID */
    ThreadId tid = 0;
    /*! CPU Index */
    CpuId cpu = 0;
    /*! Group to which the process belongs. */
    GroupId cgroup = 0;
    /*! Time of the sample. */
    TimePoint time = TimePoint::zero();
};

/*! Sampling termination reason. */
enum class SamplingTermReason {
    Time,  /*!< Sampling time expired. */
    Token, /*!< Termination requested by std::stop_token. */
    Error, /*!< Terminated due to an unrecoverable error. */
};

/*******************************************************************************
 * struct SamplingResult - Result of sampling gatherer
 ******************************************************************************/
struct SamplingResult {
    /*! Collected samples. */
    std::vector<Sample> samples;
    /**
     * @brief Map processes ids to names.
     *
     * Not all PIDs present in @ref samples must be present here, if not
     * the process name is unknown.
     * That can happen for short-time processes where we did fail to obtain
     * the name before process exited.
     */
    std::unordered_map<ProcId, std::string> process_names;
    /*! Reason for termination. */
    SamplingTermReason term_reason;
};

/*******************************************************************************
 * @brief Sample system for what processes are executed.
 *
 * Synchronous call - takes roughly duration seconds, use @p token to unblock
 * the call.
 * If cancelled, samples collected before cancellation are returned.
 *
 * @param duration Approximately how to sample.
 * @param sample_frequency Samples to take per second.
 * @param token Cancel the sampling early.
 * @return Collected samples.
 ******************************************************************************/
SamplingResult
sample_execution_sync(std::chrono::seconds duration, std::size_t sample_frequency, const std::stop_token& token);

} // namespace elphi
