#include <catch2/catch_all.hpp>
#include <catch2/matchers/catch_matchers_all.hpp>
#include <elphi/sampler.hpp>


using namespace std::chrono_literals;
SCENARIO("Synchronous sampling of system execution", "[sampling]") {
    auto sampling_time = 5s;
    std::size_t sampling_freq = 5;
    std::stop_token token;

    GIVEN("Requested stop beforehand") {
        std::stop_source source;
        token = source.get_token();
        source.request_stop();

        WHEN("sampling starts") {
            auto result = elphi::sample_execution_sync(sampling_time, sampling_freq, token);

            THEN("sampling terminates with no samples") {
                CHECK(result.term_reason == elphi::SamplingTermReason::Token);
                CHECK_THAT(result.samples, Catch::Matchers::IsEmpty());
                CHECK_THAT(result.process_names, Catch::Matchers::IsEmpty());
            }
        }
    }

    GIVEN("Zero sampling time") {
        auto sampling_time = 0s;

        WHEN("sampling starts") {
            auto result = elphi::sample_execution_sync(sampling_time, sampling_freq, token);

            THEN("sampling terminates with no samples") {
                CHECK(result.term_reason == elphi::SamplingTermReason::Time);
                CHECK_THAT(result.samples, Catch::Matchers::IsEmpty());
                CHECK_THAT(result.process_names, Catch::Matchers::IsEmpty());
            }
        }
    }
    GIVEN("Sample for 1 second") {
        auto sampling_time = 1s;

        WHEN("sampling starts") {
            auto result = elphi::sample_execution_sync(sampling_time, sampling_freq, token);

            THEN("sampling terminates with an error") {
                CHECK(result.term_reason == elphi::SamplingTermReason::Error);
                CHECK_THAT(result.samples, Catch::Matchers::IsEmpty());
                CHECK_THAT(result.process_names, Catch::Matchers::IsEmpty());
            }
        }
    }
}
