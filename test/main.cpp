#define CATCH_CONFIG_MAIN
#include <catch2/catch_all.hpp>
#include <wordle/wordle.h>

using namespace Project;

// https://stackoverflow.com/questions/71324956/wordle-implementation-dealing-with-duplicate-letters-edge-case
TEST_CASE("Wordle", "[from stackoverflow]") {
    auto w = Wordle::Open("words.txt").unwrap();
    auto s = w.start_guess_session("close", false).unwrap();

    auto a = s.guess("cheer").unwrap();
    REQUIRE(a[0] == Wordle::GuessScore::GREEN);
    REQUIRE(a[1] == Wordle::GuessScore::GREY);
    REQUIRE(a[2] == Wordle::GuessScore::YELLOW);
    REQUIRE(a[3] == Wordle::GuessScore::GREY);
    REQUIRE(a[4] == Wordle::GuessScore::GREY);
    REQUIRE(s.hints.at('e') == Wordle::GuessScore::YELLOW);

    auto b = s.guess("cocks").unwrap();
    REQUIRE(b[0] == Wordle::GuessScore::GREEN);
    REQUIRE(b[1] == Wordle::GuessScore::YELLOW);
    REQUIRE(b[2] == Wordle::GuessScore::GREY);
    REQUIRE(b[3] == Wordle::GuessScore::GREY);
    REQUIRE(b[4] == Wordle::GuessScore::YELLOW);
    REQUIRE(s.hints.at('e') == Wordle::GuessScore::YELLOW);

    auto c = s.guess("leave").unwrap();
    REQUIRE(c[0] == Wordle::GuessScore::YELLOW);
    REQUIRE(c[1] == Wordle::GuessScore::GREY);
    REQUIRE(c[2] == Wordle::GuessScore::GREY);
    REQUIRE(c[3] == Wordle::GuessScore::GREY);
    REQUIRE(c[4] == Wordle::GuessScore::GREEN);
    REQUIRE(s.hints.at('e') == Wordle::GuessScore::GREEN);
}

TEST_CASE("Wordle", "[hard mode]") {
    auto w = Wordle::Open("words.txt").unwrap();
    auto s = w.start_guess_session("train", true).unwrap();

    std::ignore = s.guess("chalk");
    REQUIRE(s.guess("maker").is_err()); // `a` must be in 3rd
    REQUIRE(s.guess("awake").is_ok());
    REQUIRE(s.guess("quark").is_ok()); // got `r` in wrong position
    REQUIRE(s.guess("chase").is_err()); // must include `r`
}

