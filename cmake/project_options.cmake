# Set reasonable build options for Elphi project targets
function(config_default_target_flags target)
  target_compile_options(${target} PRIVATE -std=c++20 -Wall -Wextra -Werror -pedantic -fpic -fno-omit-frame-pointer)
  target_compile_options(${target} PRIVATE -D_POSIX_C_SOURCE=200809)

  if(ELPHI_ASAN)
    target_compile_options(${target} PRIVATE -fsanitize=address,leak,undefined)
    target_link_options(${target} PRIVATE -fsanitize=address,leak,undefined)
  endif(ELPHI_ASAN)

  if(ELPHI_MSAN)
    target_compile_options(${target} PRIVATE -fsanitize=memory)
    target_link_options(${target} PRIVATE -fsanitize=memory)
  endif(ELPHI_MSAN)

  if(ELPHI_TSAN)
    target_compile_options(${target} PRIVATE -fsanitize=thread)
    target_link_options(${target} PRIVATE -fsanitize=thread)
  endif(ELPHI_TSAN)

  if(CMAKE_BUILD_TYPE STREQUAL Debug)
    target_compile_options(${target} PRIVATE -O0 -g3)
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
