add_executable(tui)
add_executable(elphi::tui ALIAS tui)

config_default_target_flags(tui)

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

target_link_libraries(
  tui PRIVATE
  elphi::elphi
  ftxui::screen
  ftxui::dom
  ftxui::component
)
