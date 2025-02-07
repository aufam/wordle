#include <fmt/format.h>
#include <wordle/wordle.h>
#include <fstream>
#include <random>
#include <algorithm>

using namespace Project;
using delameta::Result, delameta::Error;
using etl::Ok, etl::Err;

static auto check_valid_word(const std::string& word, uint32_t number_of_letters) -> Result<void>;
static auto check_database(const std::string& words_database, const std::string& word_to_check) -> Result<void>;

Wordle::Wordle(
    const std::string& words_database,
    uint32_t number_of_letters,
    std::vector<std::streampos>&& line_offsets
)
    : words_database(words_database)
    , number_of_letters(number_of_letters)
    , line_offsets(std::move(line_offsets))
{}

Wordle::Wordle(Wordle&& other) noexcept
    : words_database(std::move(other.words_database))
    , number_of_letters(other.number_of_letters)
    , line_offsets(std::move(other.line_offsets))
{}

auto Wordle::Open(const std::string& words_database, uint32_t number_of_letters) -> Result<Wordle> {
    std::ifstream file(words_database);
    if (!file) return Err("Unable to open file");

    std::vector<std::streampos> line_offsets;
    line_offsets.push_back(0); // first line starts at byte 0

    std::string word;
    for (int line = 0; std::getline(file, word); ++line) {
        if (auto [valid, err] = check_valid_word(word, number_of_letters); not valid)
            return Err(Error{err->code, fmt::format("{} Error at {}:{}.", err->what, words_database, line + 1)});

        line_offsets.push_back(file.tellg());
    }

    return Ok(Wordle(words_database, number_of_letters, std::move(line_offsets)));
}

auto Wordle::pick_random() const -> std::string {
    auto rng = std::mt19937(static_cast<unsigned>(std::time(nullptr)));
    auto dist = std::uniform_int_distribution<size_t>(0, line_offsets.size() - 1);
    size_t target_index = dist(rng);

    if (std::ifstream file(words_database); file.is_open()) {
        file.seekg(line_offsets[target_index]);
        std::string word;
        std::getline(file, word);
        return word;
    } else {
        fmt::println(stderr, "Fatal Error: Unable to open file `{}`. Maybe the file is missing?", words_database);
        return "";
    }
}

auto Wordle::start_guess_session(const std::string& target_word, bool hard_mode) const -> Result<Wordle::GuessSession> {
    return check_valid_word(target_word, number_of_letters)
        .and_then([&]() {
            return check_database(words_database, target_word);
        }).then([&]() {
            return GuessSession{this, target_word, hard_mode};
        });
}

auto Wordle::pick_random_and_start_guess_session(bool hard_mode) const -> Wordle::GuessSession {
    return {this, pick_random(), hard_mode};
}

auto Wordle::GuessSession::guess(const std::string& guess_word) -> Result<Wordle::GuessResult> {
    auto [valid, err] =
        check_valid_word(guess_word, wordle->number_of_letters)
        .and_then([&]() {
            return check_database(wordle->words_database, guess_word);
        });

    if (not valid)
        return Err(std::move(*err));

    // Step 0: Check for hard mode
    if (hard_mode) {
        for (auto [pos, c]: green_hints) if (guess_word[pos] != c)
            return Err(Error{-1,
                fmt::format("{}{} letter must be {}.",
                    pos + 1,
                    pos == 0 ? "st" : pos == 1 ? "nd" : pos == 2 ? "rd" : "th", // assuming the word length is less than 21
                    (char)std::toupper(c)
                )
            });

        for (auto [c, score]: hints) if (score == Wordle::GuessScore::YELLOW && guess_word.find(c) == std::string::npos)
            return Err(Error{-1,
                fmt::format("Guess word must contain {}.", (char)std::toupper(c))
            });
    }

    Wordle::GuessResult guess_result(wordle->number_of_letters, Wordle::GuessScore::GREY);
    std::unordered_map<char, int> letter_count;

    // Step 1: Count occurrences of each letter in the target word
    for (char c: target_word) letter_count[c]++;

    // Step 2: Mark correct positions first (Green)
    int pos = 0;
    for (auto [guess_c, target_c, score]: etl::zip(guess_word, target_word, guess_result)) {
        if (guess_c == target_c) {
            score = Wordle::GuessScore::GREEN;
            letter_count[guess_c]--; // Reduce available count
            green_hints[pos] = guess_c;
            hints[guess_c] = score;
        } else {
            hints.emplace(guess_c, score); // Add score only if the char is not in the map
        }

        ++pos;
    }

    // Step 3: Mark misplaced letters (Yellow) if there are remaining instances
    for (auto [guess_c, target_c, score]: etl::zip(guess_word, target_word, guess_result)) {
        if (score == Wordle::GuessScore::GREY) {
            if (letter_count[guess_c] > 0){
                score = Wordle::GuessScore::YELLOW;
                letter_count[guess_c]--; // Reduce count to prevent overmarking
                hints[guess_c] = score;
            }
        }
    }

    guessed_words.push_back(guess_word);
    return Ok(std::move(guess_result));
}

static auto check_valid_word(const std::string& word, uint32_t number_of_letters) -> Result<void> {
    if (word.size() != number_of_letters) return Err(Error{
        -1,
        fmt::format("Word `{}` is unacceptable. The word length must be {}.", word, number_of_letters),
    });

    if (
        std::any_of(word.begin(), word.end(), [](char c) {
            return not std::isalpha(c) or std::isupper(c);
        })
    ) return Err(Error{
        -1,
        fmt::format("Word `{}` is unacceptable. The letters must be all alphanumeric and lowercase.", word),
    });

    return Ok();
}

static auto check_database(const std::string& words_database, const std::string& word_to_check) -> Result<void> {
    if (std::ifstream file(words_database); file.is_open()) {
        std::string word;
        bool found = false;

        while (file >> word) if (found = (word == word_to_check); found) break;

        if (not found) return Err("The word is not in the list");
    } else {
        return Err("Unable to open file");
    }

    return Ok();
}
