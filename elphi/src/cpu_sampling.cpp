/*******************************************************************************
 * @file cpu_sampler.cpp
 * @copyright Copyright 2022 Jan Waltl.
 * @license	This file is released under ElPhi project's license, see LICENSE.
 *
 * Example on how to sample the system activity.
 ******************************************************************************/

#include <cassert>
#include <future>
#include <thread>

#include <fmt/core.h>

#include <elphi/cpu_sampler.hpp>

using namespace std::chrono_literals;

constexpr std::size_t frequency = 10;
constexpr auto c_duration = 5000ms;

int
main() {
    try {
        std::promise<elphi::CpuSamplingResult> promise;
        auto fut = promise.get_future();
        std::jthread sampling{[&](const auto& tok) {
            promise.set_value(elphi::sample_cpus_sync(std::vector<elphi::CpuId>{0}, frequency, tok));
        }};

        std::this_thread::sleep_for(c_duration);
        sampling.request_stop();
        sampling.join();
        auto samples = fut.get();

        fmt::print("Number of samples {}\n", samples.samples.size());
    } catch (const elphi::ElphiException& e) {
        fmt::print("EXCEPTION {}\n", e.what());
    }
}
