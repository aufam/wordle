#include <wordle/wordle.h>
#include <ftxui/component/component.hpp>

using namespace Project;
using namespace ftxui;

static const std::unordered_map<Wordle::GuessScore, Color> color_map = {
    {Wordle::GuessScore::GREY,   Color::Grey0},
    {Wordle::GuessScore::YELLOW, Color::Yellow},
    {Wordle::GuessScore::GREEN,  Color::Green},
};

auto background_color(Wordle::GuessScore score) -> Decorator {
    return bgcolor(color_map.at(score));
}

