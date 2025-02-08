#include <wordle/wordle.h>
#include <ftxui/component/component.hpp>

using namespace Project;
using namespace ftxui;

extern void popup(std::string title, std::string message);

bool event_handler(const Event& e, Wordle::GuessSession& s, std::string& word_builder) {
    if (e.is_character() && std::isalpha(e.character()[0])) {
        if (word_builder.size() < s.wordle->number_of_letters) {
            word_builder += (char)std::tolower(e.character()[0]);
        }
        return true;
    }
    else if (e == Event::Backspace or e.input() == "Delete") {
        if (not word_builder.empty()) word_builder.pop_back();
        return true;
    }
    else if (e == Event::Return or e.input() == "Enter") {
        if (word_builder.size() == s.wordle->number_of_letters) {
            auto [_, err] = s.guess(word_builder);
            if (err) popup("Error", err->what);
            else word_builder = "";
        }
        return true;
    }

    return false;
}

