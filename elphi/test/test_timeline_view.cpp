#include <catch2/catch_all.hpp>
#include <catch2/matchers/catch_matchers_all.hpp>
#include <elphi/timeline_view.hpp>


using namespace std::chrono_literals;
namespace velphi = elphi::view;
namespace {
velphi::ThreadTimeSlice
sample_to_timeslice(const elphi::Sample& sample, std::string name) {
    return {
        .begin_time = sample.time,
        .end_time = sample.time,
        .name = std::move(name),
        .pid = sample.pid,
        .tid = sample.tid,
        .cpu = sample.cpu,
        .cgroup = sample.cgroup,
    };
}
} // namespace

SCENARIO("Timeline view single sample processing", "[view][timeline]") {

    GIVEN("A named sample") {
        constexpr elphi::Sample sample{.pid = 1, .tid = 4, .cpu = 3, .cgroup = 2, .time = 1s};
        const std::string proc_name = "cat";
        elphi::SamplingResult result{.samples = {sample},
                                     .process_names = {{sample.pid, proc_name}},
                                     .term_reason = elphi::SamplingTermReason::Time};

        WHEN("Processed") {
            velphi::Timeline timeline = velphi::gen_cpu_timelines(result);
            THEN("Timeline contains exactly the one sample") {
                const auto exp_slice = sample_to_timeslice(sample, proc_name);

                CHECK(timeline.contains(sample.cpu));
                CHECK_THAT(timeline, Catch::Matchers::SizeIs(1));
                CHECK_THAT(timeline[sample.cpu], Catch::Matchers::SizeIs(1));
                CHECK(timeline[sample.cpu][0] == exp_slice);
            }
        }
    }

    GIVEN("An unnamed sample") {
        constexpr elphi::Sample sample{.pid = 12, .tid = 41, .cpu = 31, .cgroup = 21, .time = 11s};
        elphi::SamplingResult result{
            .samples = {sample}, .process_names = {}, .term_reason = elphi::SamplingTermReason::Time};

        WHEN("Processed") {
            velphi::Timeline timeline = velphi::gen_cpu_timelines(result);
            THEN("Timeline contains exactly the one unnamed sample") {
                const auto exp_slice = sample_to_timeslice(sample, "UNKNOWN");

                CHECK(timeline.contains(sample.cpu));
                CHECK_THAT(timeline, Catch::Matchers::SizeIs(1));
                CHECK_THAT(timeline[sample.cpu], Catch::Matchers::SizeIs(1));
                CHECK(timeline[sample.cpu][0] == exp_slice);
            }
        }
    }
}

SCENARIO("Timeline view sample concatenation", "[view][timeline]") {

    GIVEN("Two samples from same process and thread") {

        constexpr elphi::Sample sample1{.pid = 1, .tid = 4, .cpu = 3, .cgroup = 2, .time = 1s};
        constexpr elphi::Sample sample2{.pid = 1, .tid = 4, .cpu = 3, .cgroup = 2, .time = 5s};
        const std::string proc_name = "cat";
        elphi::SamplingResult result{.samples = {sample1, sample2},
                                     .process_names = {{1, proc_name}},
                                     .term_reason = elphi::SamplingTermReason::Time};
        WHEN("Processed") {
            velphi::Timeline timeline = velphi::gen_cpu_timelines(result);
            THEN("Samples are merged in the timeline") {
                auto exp_slice = sample_to_timeslice(sample1, proc_name);
                // Merged
                exp_slice.end_time = sample2.time;

                CHECK(timeline.contains(sample1.cpu));
                CHECK_THAT(timeline, Catch::Matchers::SizeIs(1));
                CHECK_THAT(timeline[sample1.cpu], Catch::Matchers::SizeIs(1));
                CHECK(timeline[sample1.cpu][0] == exp_slice);
            }
        }
    }

    GIVEN("Two samples from same process but different thread") {

        constexpr elphi::Sample sample1{.pid = 1, .tid = 4, .cpu = 3, .cgroup = 2, .time = 1s};
        constexpr elphi::Sample sample2{.pid = 1, .tid = 5, .cpu = 3, .cgroup = 2, .time = 5s};
        const std::string proc_name = "cat";
        elphi::SamplingResult result{.samples = {sample1, sample2},
                                     .process_names = {{1, proc_name}},
                                     .term_reason = elphi::SamplingTermReason::Time};
        WHEN("Processed") {
            velphi::Timeline timeline = velphi::gen_cpu_timelines(result);
            THEN("Samples appear as two slices in the timeline") {
                auto exp_slice1 = sample_to_timeslice(sample1, proc_name);
                auto exp_slice2 = sample_to_timeslice(sample2, proc_name);

                CHECK(timeline.contains(sample1.cpu));
                CHECK_THAT(timeline, Catch::Matchers::SizeIs(1));
                CHECK_THAT(timeline[sample1.cpu], Catch::Matchers::SizeIs(2));
                CHECK(timeline[sample1.cpu][0] == exp_slice1);
                CHECK(timeline[sample2.cpu][1] == exp_slice2);
            }
        }
    }
    GIVEN("Two samples from different process but same thread") {

        constexpr elphi::Sample sample1{.pid = 1, .tid = 4, .cpu = 3, .cgroup = 2, .time = 1s};
        constexpr elphi::Sample sample2{.pid = 2, .tid = 4, .cpu = 3, .cgroup = 2, .time = 5s};
        const std::string proc_name = "cat";
        elphi::SamplingResult result{.samples = {sample1, sample2},
                                     .process_names = {{1, proc_name}},
                                     .term_reason = elphi::SamplingTermReason::Time};
        WHEN("Processed") {
            velphi::Timeline timeline = velphi::gen_cpu_timelines(result);
            THEN("Samples appear as two slices in the timeline") {
                auto exp_slice1 = sample_to_timeslice(sample1, proc_name);
                auto exp_slice2 = sample_to_timeslice(sample2, "UNKNOWN");

                CHECK(timeline.contains(sample1.cpu));
                CHECK_THAT(timeline, Catch::Matchers::SizeIs(1));
                CHECK_THAT(timeline[sample1.cpu], Catch::Matchers::SizeIs(2));
                CHECK(timeline[sample1.cpu][0] == exp_slice1);
                CHECK(timeline[sample2.cpu][1] == exp_slice2);
            }
        }
    }
}
