add_executable(tui)
add_executable(elphi::tui ALIAS tui)

target_compile_options(
  tui PRIVATE
  -Werror -Wall -Wextra -pedantic
)

set_target_properties(
  tui PROPERTIES
  CXX_STANDARD 17
  CXX_STANDARD_REQUIRED ON
  CXX_EXTENSIONS OFF
  POSITION_INDEPENDENT_CODE ON
  CMAKE_EXPORT_COMPILE_COMMANDS ON
)

target_include_directories(
  tui
  PRIVATE
  include
)

target_sources(
  tui
  PRIVATE
  src/main.cpp
)

target_link_libraries(tui PRIVATE elphi::elphi)
