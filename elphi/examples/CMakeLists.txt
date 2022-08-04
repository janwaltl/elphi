add_executable(cpu_sampling)
add_executable(elphi::examples::cpu_sampling ALIAS cpu_sampling)

target_compile_options(
  cpu_sampling PRIVATE
  -Werror -Wall -Wextra -pedantic
  -D_POSIX_C_SOURCE=200809
)

set_target_properties(
  cpu_sampling PROPERTIES
  CXX_STANDARD 20
  CXX_STANDARD_REQUIRED ON
  CXX_EXTENSIONS OFF
  POSITION_INDEPENDENT_CODE ON
  CMAKE_EXPORT_COMPILE_COMMANDS ON
)


find_package(Threads REQUIRED)

target_link_libraries(cpu_sampling PRIVATE elphi::elphi Threads::Threads fmt::fmt)

target_sources(
  cpu_sampling
  PRIVATE
  cpu_sampling.cpp
)
