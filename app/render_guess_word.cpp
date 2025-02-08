#include <wordle/wordle.h>
#include <ftxui/component/component.hpp>

using namespace Project;
using namespace ftxui;

extern auto background_color(Wordle::GuessScore score) -> Decorator;

static auto render_row(const std::string& word, const Wordle::GuessResult& scores) -> Element {
    Elements letters;
    for (auto [letter, score]: etl::zip(word, scores)) letters.push_back(
        text(std::string(" ") + (char)std::toupper(letter) + ' ' )
            | background_color(score)
            | border
    );

    return hbox(std::move(letters)) | center;
}

static auto render_incomplete_row(const std::string& word, int n_letters) -> Element {
    Elements letters;
    for (size_t i: etl::range(n_letters)) {
        char letter = i < word.size() ? word[i] : ' ';
        letters.push_back(
            text(std::string(" ") + (char)std::toupper(letter) + ' ' )
            | border
        );
    }

    return hbox(std::move(letters)) | center;
}

auto render_guess_word(const Wordle::GuessSession& s, const std::string& word_builder) -> Element {
    auto words = Elements();
    bool printed = false;

    for (size_t i: etl::range(6)) {
        if (i < s.past_results.size()) {
            words.push_back(render_row(s.past_results[i].first, s.past_results[i].second));
        } else if (not printed) {
            words.push_back(render_incomplete_row(word_builder, s.wordle->number_of_letters));
            printed = true;
        } else {
            words.push_back(render_incomplete_row("", s.wordle->number_of_letters));
        }
    }

    return vbox(std::move(words)) | center;
}

