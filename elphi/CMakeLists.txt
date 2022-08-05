add_library(elphi STATIC)
add_library(elphi::elphi ALIAS elphi)

config_default_target_flags(elphi)

target_include_directories(
  elphi
  PUBLIC
  include
)

target_link_libraries(elphi PRIVATE fmt::fmt)

target_sources(
  elphi
  PUBLIC
  include/elphi/exception.hpp
  include/elphi/cpu_sampler.hpp
  include/elphi/timeline_view.hpp
  include/elphi/utils.hpp
  include/elphi/perf_events.hpp
  include/elphi/file_descriptor.hpp
  PRIVATE
  src/cpu_sampler.cpp
  src/timeline_view.cpp
  src/utils.cpp
  src/perf_events.cpp
  src/file_descriptor.cpp
)

if(ELPHI_BUILD_TEST)
  add_subdirectory(test)
endif(ELPHI_BUILD_TEST)

if(ELPHI_BUILD_EXAMPLES)
  add_subdirectory(examples)
endif(ELPHI_BUILD_EXAMPLES)
