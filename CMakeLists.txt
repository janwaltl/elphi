cmake_minimum_required(VERSION 3.15..3.18)

# Project global options
project(elphi VERSION 0.1 LANGUAGES CXX)

option(ELPHI_ENABLE_COVERAGE "Enable code coverage testing support" OFF)
option(ELPHI_JUNIT_TEST_OUTPUT "Generate test reports in JUNIT(XML) format" OFF)
option(ELPHI_BUILD_EXAMPLES "Build examples" OFF)
option(ELPHI_BUILD_TEST "Build tests" OFF)
option(ELPHI_ASAN "Sanitized build (addr,leak,UB)" OFF)
option(ELPHI_MSAN "Sanitized build (memory)" OFF)
option(ELPHI_TSAN "Sanitized build (thread)" OFF)

set_property(GLOBAL PROPERTY USE_FOLDERS ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_CXX_STANDARD 20)

include(cmake/project_options.cmake)

# External libs
add_subdirectory(libs/catch2)
add_subdirectory(libs/fmt)


if(ELPHI_ENABLE_COVERAGE)
  include(cmake/CodeCoverage.cmake)
  set(GCOVR_ADDITIONAL_ARGS --xml "coverage.xml")
  setup_target_for_coverage_gcovr_html(
    NAME coverage
    EXECUTABLE ctest -C
    DEPENDENCIES elphi::elphi elphi::elphi_tests
    EXCLUDE "elphi/test/*"
    EXCLUDE "elphi/examples/*"
    EXCLUDE "libs*"
  )
endif(ELPHI_ENABLE_COVERAGE)

if(ELPHI_BUILD_TEST)
  enable_testing()
endif(ELPHI_BUILD_TEST)

add_subdirectory(elphi)
add_subdirectory(tui)
