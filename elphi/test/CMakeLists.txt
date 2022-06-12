add_executable(elphi_tests)
add_executable(elphi::elphi_tests ALIAS elphi_tests)

target_link_libraries(elphi_tests PRIVATE elphi::elphi)
target_link_libraries(elphi_tests PRIVATE Catch2::Catch2WithMain)

target_compile_options(
  elphi_tests PRIVATE
  -Werror -Wall -Wextra -pedantic
)
target_link_options(
  elphi_tests PRIVATE
  -Wl,--wrap=mmap
  -Wl,--wrap=munmap
  -Wl,--wrap=close
)

include(CTest)
include(Catch)

make_directory("${CMAKE_BINARY_DIR}/reports")
if(JUNIT_TEST_OUTPUT)
  catch_discover_tests(
    elphi_tests
    OUTPUT_DIR "${CMAKE_BINARY_DIR}/reports"
    OUTPUT_SUFFIX ".xml"
    REPORTER junit
  )
else()
  catch_discover_tests(
    elphi_tests
    EXTRA_ARGS --colour-mode ansi
    OUTPUT_DIR "${CMAKE_BINARY_DIR}/reports"
    OUTPUT_SUFFIX ".txt"
  )
endif(JUNIT_TEST_OUTPUT)

if(ENABLE_COVERAGE)
  if(NOT CMAKE_BUILD_TYPE STREQUAL "Debug")
    message(WARNING "Running coverage for non-debug build.")
  endif(NOT CMAKE_BUILD_TYPE STREQUAL "Debug")
  target_compile_options(elphi_tests PRIVATE --coverage)
  target_link_options(elphi_tests PRIVATE --coverage)
endif(ENABLE_COVERAGE)

set_target_properties(
  elphi_tests PROPERTIES
  CXX_STANDARD 20
  CXX_STANDARD_REQUIRED ON
  CXX_EXTENSIONS OFF
  POSITION_INDEPENDENT_CODE ON
  CMAKE_EXPORT_COMPILE_COMMANDS ON
)

target_sources(
  elphi_tests
  PRIVATE
  test_sampler.cpp
  test_timeline_view.cpp
  test_utils.cpp
  test_perf_events.cpp
  mock_syscalls.cpp
)
