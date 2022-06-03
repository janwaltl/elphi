add_executable(elphi_tests test_hello.cpp)
add_executable(elphi::elphi_tests ALIAS elphi_tests)

target_link_libraries(elphi_tests PRIVATE elphi::elphi)
target_link_libraries(elphi_tests PRIVATE Catch2::Catch2WithMain)

include(CTest)
include(Catch)
catch_discover_tests(elphi_tests)
