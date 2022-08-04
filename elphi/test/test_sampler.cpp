#include <catch2/catch_all.hpp>
#include <catch2/matchers/catch_matchers_all.hpp>
#include <elphi/cpu_sampler.hpp>


using namespace std::chrono_literals;
SCENARIO("Synchronous sampling of system execution", "[sampling]") {
    std::size_t sampling_freq = 5;
    std::stop_token token;

    GIVEN("Requested stop beforehand") {
        std::stop_source source;
        token = source.get_token();
        source.request_stop();

        WHEN("sampling starts") {
            auto result = elphi::sample_cpus_sync({}, sampling_freq, token);

            THEN("sampling terminates with no samples") {
                CHECK_THAT(result.samples, Catch::Matchers::IsEmpty());
                CHECK_THAT(result.process_names, Catch::Matchers::IsEmpty());
            }
        }
    }
}
