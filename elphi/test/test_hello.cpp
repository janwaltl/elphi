#include <catch2/catch_test_macros.hpp>
#include <elphi/hello.hpp>

TEST_CASE("Hello returns", "") { REQUIRE_NOTHROW(hello()); }
