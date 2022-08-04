/*******************************************************************************
 * @file cpu_sampler.hpp
 * @copyright Copyright 2022 Jan Waltl.
 * @license	This file is released under ElPhi project's license, see LICENSE.
 *
 * Simple API for basic sampling of CPU cores for executed processes.
 ******************************************************************************/
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
struct CpuSample {
    /*! Process ID */
    ProcId pid = 0;
    /*! Thread ID */
    ThreadId tid = 0;
    /*! CPU Index */
    CpuId cpu = 0;
    /*! Time of the sample. */
    TimePoint time = TimePoint::zero();
};


/*******************************************************************************
 * struct SamplingResult - Result of sampling gatherer
 ******************************************************************************/
struct CpuSamplingResult {

    /*! Collected samples. */
    std::vector<CpuSample> samples;
    /**
     * @brief Map processes ids to names.
     *
     * Not all PIDs present in @ref samples must be present here, if not
     * the process name is unknown.
     * That can happen for short-time processes where we did fail to obtain
     * the name before process exited.
     */
    std::unordered_map<ProcId, std::string> process_names;
};

/*******************************************************************************
 * @brief Sample system for what processes are executed.
 *
 * Synchronous call the call, must be cancelled through @p token.
 *
 * @param sample_frequency Samples to take per second.
 * @param token Cancel the sampling.
 * @return Collected samples.
 * @throw ElphiException in case of errors.
 ******************************************************************************/
CpuSamplingResult
sample_cpus_sync(const std::vector<CpuId>& cpus, std::size_t sample_frequency, const std::stop_token& token);

} // namespace elphi
