cmake_minimum_required(VERSION 3.15..3.18)

project(elphi VERSION 0.1 LANGUAGES CXX)

option(ENABLE_COVERAGE "Enable code coverage testing support")
option(JUNIT_TEST_OUTPUT "Generate test reports in JUNIT(XML) format")

set(CMAKE_CXX_STANDARD 20)
set_property(GLOBAL PROPERTY USE_FOLDERS ON)

add_subdirectory(libs/catch2)
add_subdirectory(libs/fmt)

if(ENABLE_COVERAGE)
  include(cmake/CodeCoverage.cmake)
  set(GCOVR_ADDITIONAL_ARGS --xml "coverage.xml")
  setup_target_for_coverage_gcovr_html(
    NAME coverage
    EXECUTABLE ctest -C
    DEPENDENCIES elphi::elphi elphi::elphi_tests
    EXCLUDE "elphi/test/test_*"
    EXCLUDE "libs*"
  )
endif(ENABLE_COVERAGE)

enable_testing()

add_subdirectory(elphi)
add_subdirectory(tui)
