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

extern auto create_keyboard(ScreenInteractive&, const Wordle::GuessSession&) -> Component;
extern auto render_guess_word(const Wordle::GuessSession& s, const std::string& word_builder) -> Element;
extern bool event_handler(const Event& e, Wordle::GuessSession& s, std::string& word_builder);
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
    auto s = target_word.empty() ?
        w.pick_random_and_start_guess_session(hard_mode) :
        TRY(w.start_guess_session(target_word, hard_mode));

    auto screen = ScreenInteractive::Fullscreen();
    auto keyboard = create_keyboard(screen, s);
    auto word_builder = std::string();

    auto renderer = Renderer(keyboard, [&] {
        return window(
            text(std::string(" Wordle ") + (hard_mode ? " -- Hard Mode " : "-- Easy Mode ")) | bold | center,
            hbox({
                filler(),
                render_guess_word(s, word_builder),
                filler(),
                keyboard->Render() | center,
                filler(),
            })
        );
    });

    renderer |= CatchEvent([&](Event e) {
        if (not event_handler(e, s, word_builder))
            return false;

        const auto guess_count = s.past_results.size();
        const auto *last_score = guess_count ? &s.past_results.back().second : nullptr;
        const bool all_green = last_score and
            std::all_of(last_score->begin(), last_score->end(), [](auto score) {
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

        return true;
    });

    screen.Loop(renderer);
    return Ok();
}

