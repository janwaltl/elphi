add_library(elphi STATIC)
add_library(elphi::elphi ALIAS elphi)

target_compile_options(
  elphi PRIVATE
  -Werror -Wall -Wextra -pedantic
)

set_target_properties(
  elphi PROPERTIES
  CXX_STANDARD 20
  CXX_STANDARD_REQUIRED ON
  CXX_EXTENSIONS OFF
  POSITION_INDEPENDENT_CODE ON
  CMAKE_EXPORT_COMPILE_COMMANDS ON
)

if(ENABLE_COVERAGE)
  if(NOT CMAKE_BUILD_TYPE STREQUAL "Debug")
    message(WARNING "Running coverage for non-debug build.")
  endif(NOT CMAKE_BUILD_TYPE STREQUAL "Debug")
  target_compile_options(elphi PRIVATE --coverage)
  target_link_options(elphi PRIVATE --coverage)
endif(ENABLE_COVERAGE)

target_include_directories(
  elphi
  PUBLIC
  include
)

target_link_libraries(elphi PRIVATE fmt::fmt)

target_sources(
  elphi
  PUBLIC
  include/elphi/exception.hpp
  include/elphi/sampler.hpp
  include/elphi/timeline_view.hpp
  include/elphi/utils.hpp
  PRIVATE
  src/sampler.cpp
  src/timeline_view.cpp
  src/utils.cpp
)

add_subdirectory(test)
