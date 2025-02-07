#include <wordle/wordle.h>
#include <ftxui/component/component.hpp>

using namespace Project;
using namespace ftxui;
using delameta::Result;
using etl::Ok, etl::Err;

struct WordBuilder {
    std::string word;
    Wordle::GuessResult scores;
};

auto event_handler(Wordle::GuessSession& s, Event e, WordBuilder& builder, int& guess_count) -> std::pair<Result<void>, bool> {
    if (e.is_character() && std::isalpha(e.character()[0])) {
        if (builder.word.size() < s.wordle->number_of_letters) {
            builder.word += (char)std::tolower(e.character()[0]);
        }
        return {Ok(), true};
    }
    else if (e == Event::Backspace or e.input() == "Delete") {
        if (not builder.word.empty()) builder.word.pop_back();
        return {Ok(), true};
    }
    else if (e == Event::Return or e.input() == "Enter") {
        if (builder.word.size() == s.wordle->number_of_letters) {
            auto [scores, err] = s.guess(builder.word);
            if (err) return {Err(std::move(*err)), true};

            builder.scores = std::move(*scores);
            ++guess_count;
        }
        return {Ok(), true};
    }

    return {Ok(), false};
}
