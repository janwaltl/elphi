add_library(elphi STATIC)
add_library(elphi::elphi ALIAS elphi)

target_compile_options(
  elphi PRIVATE
  -Werror -Wall -Wextra -pedantic
)

set_target_properties(
  elphi PROPERTIES
  CXX_STANDARD 17
  CXX_STANDARD_REQUIRED ON
  CXX_EXTENSIONS OFF
  POSITION_INDEPENDENT_CODE ON
  CMAKE_EXPORT_COMPILE_COMMANDS ON
)

target_include_directories(
  elphi
  PUBLIC
  include
)

target_sources(
  elphi
  PUBLIC
  include/elphi/hello.hpp
  PRIVATE
  src/hello.cpp
)
