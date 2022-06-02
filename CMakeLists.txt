cmake_minimum_required(VERSION 3.15..3.18)

project(elphi VERSION 0.1 LANGUAGES CXX)

set_property(GLOBAL PROPERTY USE_FOLDERS ON)

add_executable(elphi)

target_compile_options(elphi PRIVATE 
	-Werror -Wall -Wextra -pedantic
)

set_target_properties(elphi PROPERTIES
	CXX_STANDARD 17
	CXX_STANDARD_REQUIRED ON
	CXX_EXTENSIONS OFF
	POSITION_INDEPENDENT_CODE ON
)

target_include_directories(elphi
	PRIVATE
	include
)

target_sources(elphi
	PRIVATE
	include/elphi/hello.hpp
	src/main.cpp
	src/hello.cpp
)
