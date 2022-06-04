#pragma once

#include <elphi/sampler.hpp>

namespace elphi::view {


/*******************************************************************************
 * struct ThreadTimeSlice - Time slice executing the given thread.
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
    /*! Group to which the process belongs. */
    GroupId cgroup = 0;

    friend auto
    operator<=>(const ThreadTimeSlice&, const ThreadTimeSlice&) = default;
};

/*! Timeline for a single CPU. */
using CpuTimeline = std::vector<ThreadTimeSlice>;
/*! Timeline for each CPU. */
using Timeline = std::unordered_map<CpuId, CpuTimeline>;
/*! Timeline for each cgroup. */
using GroupTimeline = std::unordered_map<GroupId, Timeline>;

Timeline
gen_cpu_timelines(const SamplingResult& result);
} // namespace elphi::view
