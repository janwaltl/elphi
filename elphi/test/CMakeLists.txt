add_executable(elphi_tests test_hello.cpp)
add_executable(elphi::elphi_tests ALIAS elphi_tests)

target_link_libraries(elphi_tests PRIVATE elphi::elphi)
target_link_libraries(elphi_tests PRIVATE Catch2::Catch2WithMain)

include(CTest)
include(Catch)

make_directory("${CMAKE_BINARY_DIR}/reports")
catch_discover_tests(
  elphi_tests
  OUTPUT_DIR "${CMAKE_BINARY_DIR}/reports"
  OUTPUT_SUFFIX ".xml"
  REPORTER junit
)

if(ENABLE_COVERAGE)
  if(NOT CMAKE_BUILD_TYPE STREQUAL "Debug")
    message(WARNING "Running coverage for non-debug build.")
  endif(NOT CMAKE_BUILD_TYPE STREQUAL "Debug")
  target_link_options(elphi_tests PRIVATE --coverage)
endif(ENABLE_COVERAGE)
