#include <ftxui/component/component.hpp>
#include <ftxui/component/screen_interactive.hpp>

using namespace ftxui;

void popup(std::string title, std::string message) {
    auto popup = Renderer([=] {
        return window(
            text(title),
            text(message)
        ) | center;
    });

    auto screen = ScreenInteractive::Fullscreen();
    popup |= CatchEvent([&](Event event) {
        bool handled =
            (event.is_mouse() && event.mouse().button == Mouse::Left && event.mouse().motion == Mouse::Pressed)
            or event.is_character()
            or event == Event::Return;

        if (handled) screen.ExitLoopClosure()();
        return handled;
    });

    screen.Loop(popup);
}
