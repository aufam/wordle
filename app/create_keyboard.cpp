#include <wordle/wordle.h>
#include <ftxui/component/component.hpp>
#include <ftxui/component/screen_interactive.hpp>

using namespace Project;
using namespace ftxui;

extern auto background_color(Wordle::GuessScore score) -> Decorator;

static const std::vector<std::pair<Event, std::string>> r1 = {
    {Event::Character('Q'), " Q "},
    {Event::Character('W'), " W "},
    {Event::Character('E'), " E "},
    {Event::Character('R'), " R "},
    {Event::Character('T'), " T "},
    {Event::Character('Y'), " Y "},
    {Event::Character('U'), " U "},
    {Event::Character('I'), " I "},
    {Event::Character('O'), " O "},
    {Event::Character('P'), " P "},
};

static const std::vector<std::pair<Event, std::string>> r2 = {
    {Event::Character('A'), " A "},
    {Event::Character('S'), " S "},
    {Event::Character('D'), " D "},
    {Event::Character('F'), " F "},
    {Event::Character('G'), " G "},
    {Event::Character('H'), " H "},
    {Event::Character('J'), " J "},
    {Event::Character('K'), " K "},
    {Event::Character('L'), " L "},
};

static const std::vector<std::pair<Event, std::string>> r3 = {
    {Event::Special("Enter"), "Enter"},
    {Event::Character('Z'), " Z "},
    {Event::Character('X'), " X "},
    {Event::Character('C'), " C "},
    {Event::Character('V'), " V "},
    {Event::Character('B'), " B "},
    {Event::Character('N'), " N "},
    {Event::Character('M'), " M "},
    {Event::Special("Delete"), "Del"},
};

auto create_keyboard(ScreenInteractive& screen, const Wordle::GuessSession& s) -> Component {
    auto create_keyboard_row = [&screen, &s](const decltype(r1)& labels) {
        Components buttons;
        for (const auto &[event, label] : labels) {
            char letter = event.is_character() ? std::tolower(event.character()[0]) : ' ';
            ButtonOption option;

            option.transform = [&s, letter](const EntryState& state) {
                auto element = text(state.label);
                if (auto it = s.hints.find(letter); it != s.hints.end())
                    element |= background_color(it->second);

                element |= border;
                if (state.focused) element |= bold;
                return element;
            };

            buttons.push_back(Button(
                label,
                [&screen, event] { screen.PostEvent(event); },
                std::move(option)
            ));
        }

        return Container::Horizontal(std::move(buttons));
    };

    return Container::Vertical({
        create_keyboard_row(r1) | center,
        create_keyboard_row(r2) | center,
        create_keyboard_row(r3) | center,
    });
}

