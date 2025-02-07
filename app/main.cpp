#include <boost/preprocessor.hpp>
#include <wordle/wordle.h>
#include <ftxui/component/component.hpp>
#include <ftxui/component/screen_interactive.hpp>
#include <delameta/opts.h>
#include <algorithm>

using namespace Project;
using namespace ftxui;
using delameta::Result;
using etl::Ok, etl::Err;

struct WordBuilder {
    std::string word;
    Wordle::GuessResult scores;
};

extern auto create_keyboard(ScreenInteractive&, const Wordle::GuessSession&) -> Component;
extern auto render_guess_word(const Wordle::GuessSession&, const WordBuilder&) -> Element;
extern auto event_handler(Wordle::GuessSession&, Event, WordBuilder&, int& guess_count) -> std::pair<Result<void>, bool>;
extern void popup(std::string title, std::string message);

OPTS_MAIN(
    (WordleMain, "Wordle")
    ,
    /*   Type   |     Arg    | Short |     Long     |              Help               |      Default   */
    (bool       , hard_mode  ,  'H'  , "hard-mode"  , "Enable hard mode"                                )
    (std::string, word_list  ,  'L'  , "word-list"  , "Specify word list file",              "words.txt")
    (std::string, target_word,  't'  , "target-word", "Specify a word as the target, or pick random", "")
    (int        , n_letters  ,  'l'  , "n-letters"  , "Number of letter in a word",                  "5")
    ,
    (Result<void>)
) {
    auto w = TRY(Wordle::Open(word_list, n_letters));
    auto s = Wordle::GuessSession();

    if (target_word.empty()) {
        s = w.pick_random_and_start_guess_session(hard_mode);
    } else {
        s = TRY(w.start_guess_session(target_word, hard_mode));
    }

    auto word_builders = std::vector<WordBuilder>(6);
    auto guess_count = 0;

    auto screen = ScreenInteractive::Fullscreen();
    auto keyboard = create_keyboard(screen, s);

    auto renderer = Renderer(keyboard, [&] {
        return window(
            text(std::string(" Wordle ") + (hard_mode ? " -- Hard Mode " : "-- Easy Mode ")) | bold | center,
            hbox({
                filler(),
                vbox({
                    render_guess_word(s, word_builders[0]),
                    render_guess_word(s, word_builders[1]),
                    render_guess_word(s, word_builders[2]),
                    render_guess_word(s, word_builders[3]),
                    render_guess_word(s, word_builders[4]),
                    render_guess_word(s, word_builders[5]),
                }) | center,
                filler(),
                keyboard->Render() | center,
                filler(),
            })
        );
    });

    renderer |= CatchEvent([&](Event e) {
        auto &builder = word_builders[guess_count];
        auto [res, handled] = event_handler(s, e, builder, guess_count);

        if (res.is_err()) popup("Error", res.unwrap_err().what);

        bool all_green = not builder.scores.empty() and
            std::all_of(builder.scores.begin(), builder.scores.end(), [](auto score) {
                return score == Wordle::GuessScore::GREEN;
            });
        if (all_green) {
            const std::string congrats_messages[] = {
                "Genius!",
                "Magnificent!",
                "Impressive!",
                "Splendid!",
                "Great!",
                "Phew!",
            };
            popup("Success", congrats_messages[guess_count - 1]);
            screen.ExitLoopClosure()();
        }
        else if (guess_count == 6) {
            std::string correct_word;
            for (char c: s.target_word) correct_word += (char)std::toupper(c);
            popup("Failed", "Correct word is " + correct_word);
            screen.ExitLoopClosure()();
        }

        return handled;
    });

    screen.Loop(renderer);
    return Ok();
}

