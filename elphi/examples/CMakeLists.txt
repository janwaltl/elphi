add_executable(cpu_sampling)
add_executable(elphi::examples::cpu_sampling ALIAS cpu_sampling)

config_default_target_flags(cpu_sampling)

find_package(Threads REQUIRED)

target_link_libraries(cpu_sampling PRIVATE elphi::elphi Threads::Threads fmt::fmt)

target_sources(
  cpu_sampling
  PRIVATE
  cpu_sampling.cpp
)
