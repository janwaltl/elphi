#include <elphi/sampler.hpp>

using namespace std::chrono_literals;

namespace elphi {
SamplingResult
sample_execution_sync(std::chrono::seconds duration, std::size_t sample_frequency, const std::stop_token& token) {
    // Stub implementation which does not actually return any samples.
    (void)duration;
    (void)sample_frequency;

    if (token.stop_requested())
        return {.samples = {}, .process_names = {}, .term_reason = SamplingTermReason::Token};

    if (duration == 0s)
        return {.samples = {}, .process_names = {}, .term_reason = SamplingTermReason::Time};

    return {.samples = {}, .process_names = {}, .term_reason = SamplingTermReason::Error};
}
} // namespace elphi
