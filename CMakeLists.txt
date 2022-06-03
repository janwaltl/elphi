cmake_minimum_required(VERSION 3.15..3.18)

project(elphi VERSION 0.1 LANGUAGES CXX)

option(ENABLE_COVERAGE "Enable code coverage testing support")

set_property(GLOBAL PROPERTY USE_FOLDERS ON)

add_subdirectory(libs/catch2)

if(ENABLE_COVERAGE)
  include(cmake/CodeCoverage.cmake)
  set(GCOVR_ADDITIONAL_ARGS --xml "coverage.xml")
  setup_target_for_coverage_gcovr_html(
    NAME coverage
    EXECUTABLE ctest -C
    DEPENDENCIES elphi::elphi elphi::elphi_tests
    BASE_DIRECTORY "../"
    EXCLUDE "*libs*"
  )
  enable_testing()
endif(ENABLE_COVERAGE)

add_subdirectory(elphi)
add_subdirectory(tui)
