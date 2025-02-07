#include <wordle/wordle.h>
#include <ftxui/component/component.hpp>

using namespace Project;
using namespace ftxui;

struct WordBuilder {
    std::string word;
    Wordle::GuessResult scores;
};

extern auto background_color(Wordle::GuessScore score) -> Decorator;

auto render_guess_word(const Wordle::GuessSession& s, const WordBuilder& builder) -> Element {
    bool already_guessed = not builder.scores.empty();
    auto letters = Elements();

    for (auto i: etl::range(s.wordle->number_of_letters)) {
        Element letter;
        if (i >= builder.word.size()) {
            letter = text("   ");
        } else {
            letter = text(std::string(" ") + (char)std::toupper(builder.word[i]) + ' ') | (
                already_guessed ? background_color(builder.scores[i]) : nothing
            );
        }

        letter |= border;
        letters.push_back(letter);
    }

    return hbox(std::move(letters)) | center;
}
