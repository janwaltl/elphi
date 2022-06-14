/*******************************************************************************
 * Example on how to sample the system activity.
 ******************************************************************************/

#include <cassert>

#include <fmt/core.h>

#include <elphi/perf_sampler.hpp>

namespace ed = elphi::data;
using namespace std::chrono_literals;


constexpr std::size_t frequency = 10;
constexpr std::size_t c_poll_timeout_ms = 2000;
constexpr auto c_sample_duration = 10s;

constexpr std::size_t c_num_cores = 6;

int
main() {
    try {
        std::vector<ed::PerfCpuSampler> samplers;
        samplers.reserve(c_num_cores);
        std::vector<pollfd> polling;
        for (std::size_t i{0}; i < c_num_cores; ++i) {
            samplers.emplace_back(i, std::nullopt, frequency);
            polling.push_back(samplers.back().get_poll_entry());
        }

        const auto now = std::chrono::steady_clock::now;
        const auto start = now();

        for (auto& s : samplers) {
            s.start();
            if (!s.is_sampling())
                return 1;
        }

        std::vector<elphi::Sample> samples;
        while ((now() - start) < c_sample_duration) {
            int num_active = poll(polling.data(), polling.size(), c_poll_timeout_ms);
            if (num_active == -1) {
                perror("Poll failed");
                return 1;
            }
            if (num_active == 0) {
                fmt::print("No activity, weird.\n");
            } else {
                for (std::size_t i{0}; i < polling.size(); ++i) {
                    auto& p = polling[i];
                    auto& s = samplers[i];
                    if (p.revents & POLLIN) {
                        for (auto maybe_sample = s.get_next(); maybe_sample; maybe_sample = s.get_next())
                            samples.push_back(*maybe_sample);
                    }
                    p.revents = 0; // Reset for next poll.
                }
            }
        }
        for (auto& s : samplers)
            s.stop();
        for (auto& s : samplers)
            for (auto maybe_sample = s.get_next(); maybe_sample.has_value(); maybe_sample = s.get_next())
                samples.push_back(*maybe_sample);

        fmt::print("Number of samples {}\n", samples.size());
    } catch (const elphi::ElphiException& e) {
        fmt::print("EXCEPTION {}\n", e.what());
    }
}
