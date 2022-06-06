#include <fmt/format.h>

#include <elphi/timeline_view.hpp>


namespace elphi::view {
namespace {

/*******************************************************************************
 * @brief Resolve process name or returns "UNKNOWN".
 ******************************************************************************/
std::string
resolve_name(const SamplingResult& result, ProcId pid) {
    auto it = result.process_names.find(pid);
    return it == result.process_names.end() ? "UNKNOWN" : it->second;
}
} // namespace

Timeline
gen_cpu_timelines(const SamplingResult& result) {

    Timeline timeline;

    for (const auto& sample : result.samples) {
        auto& cpu_timeline = timeline.try_emplace(sample.cpu).first->second;

        // Different execution context -> new slice.
        if (cpu_timeline.empty() || cpu_timeline.back().pid != sample.pid || cpu_timeline.back().tid != sample.tid) {
            cpu_timeline.push_back(ThreadTimeSlice{.begin_time = sample.time,
                                                   .end_time = sample.time,
                                                   .name = resolve_name(result, sample.pid),
                                                   .pid = sample.pid,
                                                   .tid = sample.tid,
                                                   .cpu = sample.cpu,
                                                   .cgroup = sample.cgroup});
        } else {
            // Prolong the current slice by this sample.
            cpu_timeline.back().end_time = sample.time;
        }
    }
    return timeline;
}
} // namespace elphi::view
