cmake_minimum_required(VERSION 3.15..3.18)

project(elphi VERSION 0.1 LANGUAGES CXX)

set_property(GLOBAL PROPERTY USE_FOLDERS ON)

add_subdirectory(libs/catch2)

add_subdirectory(elphi)
add_subdirectory(tui)
