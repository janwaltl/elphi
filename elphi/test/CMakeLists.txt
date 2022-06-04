add_executable(elphi_tests)
add_executable(elphi::elphi_tests ALIAS elphi_tests)

target_link_libraries(elphi_tests PRIVATE elphi::elphi)
target_link_libraries(elphi_tests PRIVATE Catch2::Catch2WithMain)

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
    EXTRA_ARGS --colour-mode ansi -d yes
    OUTPUT_DIR "${CMAKE_BINARY_DIR}/reports"
    OUTPUT_SUFFIX ".txt"
  )
endif(JUNIT_TEST_OUTPUT)

if(ENABLE_COVERAGE)
  if(NOT CMAKE_BUILD_TYPE STREQUAL "Debug")
    message(WARNING "Running coverage for non-debug build.")
  endif(NOT CMAKE_BUILD_TYPE STREQUAL "Debug")
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
  test_hello.cpp
  test_sampler.cpp
)
