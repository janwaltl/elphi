#pragma once

#include <elphi/cpu_sampler.hpp>

namespace elphi::view {

/*******************************************************************************
 * @brief Time slice executing the given thread.
 ******************************************************************************/
struct ThreadTimeSlice {
    /*! Starting of execution. */
    TimePoint begin_time;
    /*! End of execution. */
    TimePoint end_time;

    /*! Process name. */
    std::string name;

    /*! Process ID */
    ProcId pid = 0;
    /*! Thread ID */
    ThreadId tid = 0;
    /*! CPU Index */
    CpuId cpu = 0;

    /*******************************************************************************
     * @brief Default member-wise comparison.
     ******************************************************************************/
    friend auto
    operator<=>(const ThreadTimeSlice&, const ThreadTimeSlice&) = default;
};

/*! Timeline for a single CPU. */
using CpuTimeline = std::vector<ThreadTimeSlice>;
/*! Timeline for each CPU. */
using Timeline = std::unordered_map<CpuId, CpuTimeline>;
/*! Timeline for each cgroup. */
using GroupTimeline = std::unordered_map<GroupId, Timeline>;

/*******************************************************************************
 * @brief Process sampling results into cpu activity Timeline.
 *
 * Timeline shows what process each CPU executed.
 *
 * @param result Result from process sampling.
 * @return Constructed Timeline from sampling @p result.
 ******************************************************************************/
Timeline
gen_cpu_timelines(const CpuSamplingResult& result);
} // namespace elphi::view
