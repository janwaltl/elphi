cmake_minimum_required(VERSION 3.15..3.18)

project(elphi VERSION 0.1 LANGUAGES CXX)

option(ELPHI_ENABLE_COVERAGE "Enable code coverage testing support" OFF)
option(ELPHI_JUNIT_TEST_OUTPUT "Generate test reports in JUNIT(XML) format" OFF)
option(ELPHI_BUILD_EXAMPLES "Build examples" OFF)
option(ELPHI_BUILD_TEST "Build tests" OFF)

# Set reasonable build options for Elphi project targets
function(config_default_target_flags target)
  target_compile_options(${target} PRIVATE -std=c++20 -Wall -Wextra -Werror -pedantic -fpic -fno-omit-frame-pointer)
  target_compile_options(${target} PRIVATE -D_POSIX_C_SOURCE=200809)

  if(CMAKE_BUILD_TYPE STREQUAL Debug)
    target_compile_options(${target} PRIVATE -Og -g3)
  else()
    target_compile_options(${target} PRIVATE -O2 -g3)
    target_link_options(${target} PRIVATE -flto)
  endif()

  if(ELPHI_ENABLE_COVERAGE)
    if(NOT CMAKE_BUILD_TYPE STREQUAL "Debug")
      message(WARNING "Running coverage for non-debug build of \'${target}\'")
    endif(NOT CMAKE_BUILD_TYPE STREQUAL "Debug")
    target_compile_options(${target} PRIVATE --coverage)
    target_link_options(${target} PRIVATE --coverage)
  endif(ELPHI_ENABLE_COVERAGE)

endfunction()

set_property(GLOBAL PROPERTY USE_FOLDERS ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_CXX_STANDARD 20)

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
