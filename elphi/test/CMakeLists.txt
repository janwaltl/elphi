add_executable(elphi_tests)
add_executable(elphi::elphi_tests ALIAS elphi_tests)

target_link_libraries(elphi_tests PRIVATE elphi::elphi)
target_link_libraries(elphi_tests PRIVATE Catch2::Catch2WithMain)


config_default_target_flags(elphi_tests)
target_link_options(
  elphi_tests PRIVATE
  -Wl,--wrap=mmap
  -Wl,--wrap=munmap
  -Wl,--wrap=close
  -Wl,--wrap=open_perf_event
)

include(CTest)
include(Catch)

make_directory("${CMAKE_BINARY_DIR}/reports")
if(ELPHI_JUNIT_TEST_OUTPUT)
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
endif(ELPHI_JUNIT_TEST_OUTPUT)

target_sources(
  elphi_tests
  PRIVATE
  test_sampler.cpp
  test_timeline_view.cpp
  test_utils.cpp
  test_perf_events.cpp
  test_file_descriptor.cpp
  mock_syscalls.cpp
)
