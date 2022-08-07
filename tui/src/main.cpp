#include "ftxui/component/component.hpp"          // for ScreenInteractive, Component
#include "ftxui/component/screen_interactive.hpp" // for ScreenInteractive, Component

namespace gruv {
const auto red = ftxui::Color::RGB(251, 73, 52);
const auto green = ftxui::Color::RGB(184, 187, 38);
const auto yellow = ftxui::Color::RGB(250, 189, 47);
const auto blue = ftxui::Color::RGB(131, 165, 152);
const auto purple = ftxui::Color::RGB(211, 134, 155);
const auto aqua = ftxui::Color::RGB(142, 192, 124);
const auto orange = ftxui::Color::RGB(254, 128, 25);

const auto black = ftxui::Color::RGB(29, 32, 33);
const auto grey = ftxui::Color::RGB(60, 56, 54);
const auto light = ftxui::Color::RGB(235, 219, 178);
} // namespace gruv

int
main() {
    auto screen = ftxui::ScreenInteractive::Fullscreen();
    auto timeline = ftxui::vbox({
                        ftxui::text("red") | ftxui::bgcolor(gruv::red) | ftxui::center,
                        ftxui::text("green") | ftxui::bgcolor(gruv::green) | ftxui::center,
                        ftxui::text("yellow") | ftxui::bgcolor(gruv::yellow) | ftxui::center,
                        ftxui::text("blue") | ftxui::bgcolor(gruv::blue) | ftxui::center,
                        ftxui::text("purple") | ftxui::bgcolor(gruv::purple) | ftxui::center,
                        ftxui::text("aqua") | ftxui::bgcolor(gruv::aqua) | ftxui::center,
                        ftxui::text("orange") | ftxui::bgcolor(gruv::orange) | ftxui::center,
                    }) |
                    ftxui::center | ftxui::flex | ftxui::color(gruv::grey) | ftxui::bgcolor(gruv::black);
    timeline = ftxui::window(ftxui::text("Timeline") | ftxui::bold | ftxui::center, timeline);

    int value = 0;
    auto action = [&] { value++; };
    auto top = ftxui::Container::Horizontal({
                   Button("FILE", action, ftxui::ButtonOption::Ascii()),
                   Button("OPTION 1", action, ftxui::ButtonOption::Ascii()),
                   Button("OPTION 2", action, ftxui::ButtonOption::Ascii()),
                   Button("EXIT", action, ftxui::ButtonOption::Ascii()),
               }) |
               ftxui::center;

    auto status_line = ftxui::Renderer([&] {
        ++value;
        return ftxui::text("status line" + std::to_string(value)) | ftxui::center;
    });


    auto layout = ftxui::Container::Vertical({top, ftxui::Renderer([] { return ftxui::separator(); }),
                                              ftxui::Renderer([&] { return timeline; }), status_line}) |
                  ftxui::bgcolor(gruv::grey) | ftxui::color(gruv::light);

    screen.Loop(layout);
    return 0;
}
