################################################################################
# libelphi - static library
# elphi - binary
################################################################################
add_library(libelphi STATIC)
add_library(elphi::libelphi ALIAS libelphi)

config_default_target_flags(libelphi)

target_include_directories(
  libelphi
  PUBLIC
  include
)

target_link_libraries(libelphi PRIVATE fmt::fmt)

target_sources(
  libelphi
  PUBLIC
  include/elphi/exception.hpp
  include/elphi/cpu_sampler.hpp
  include/elphi/timeline_view.hpp
  include/elphi/utils.hpp
  include/elphi/perf_events.hpp
  include/elphi/file_descriptor.hpp
  PRIVATE
  lib/cpu_sampler.cpp
  lib/timeline_view.cpp
  lib/utils.cpp
  lib/perf_events.cpp
  lib/file_descriptor.cpp
)

add_executable(elphi)
add_executable(elphi::elphi ALIAS elphi)

config_default_target_flags(elphi)

find_package(Threads REQUIRED)

target_link_libraries(elphi PRIVATE elphi::libelphi Threads::Threads fmt::fmt)

target_sources(
  elphi
  PRIVATE
  src/cpu_sampling.cpp
)

if(ELPHI_BUILD_TEST)
  add_subdirectory(test)
endif(ELPHI_BUILD_TEST)
