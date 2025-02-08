#ifndef WORDLE_H
#define WORDLE_H

#include <delameta/error.h>
#include <etl/ref.h>
#include <iosfwd>
#include <vector>
#include <unordered_map>

namespace Project {
    class Wordle {
    protected:
        Wordle(
            const std::string& words_database,
            uint32_t number_of_letters,
            std::vector<std::streampos>&& line_offsets
        );
        Wordle(const Wordle&) = delete;

    public:
        Wordle(Wordle&& other) noexcept;
        static delameta::Result<Wordle> Open(const std::string& words_database, uint32_t number_of_letters = 5);

        enum class GuessScore;
        using GuessResult = std::vector<GuessScore>;
        struct GuessSession;

        std::string pick_random() const;
        delameta::Result<GuessSession> start_guess_session(const std::string& target_word, bool hard_mode) const;
        GuessSession pick_random_and_start_guess_session(bool hard_mode) const;

        std::string words_database;
        uint32_t number_of_letters;
        std::vector<std::streampos> line_offsets;
    };

    enum class Wordle::GuessScore {
        GREY,
        YELLOW,
        GREEN,
    };

    struct Wordle::GuessSession {
        etl::Ref<const Wordle> wordle;
        std::string target_word;
        bool hard_mode;
        std::vector<std::pair<std::string, Wordle::GuessResult>> past_results = {};
        std::unordered_map<char, GuessScore> hints = {};
        std::unordered_map<int, char> green_hints = {};

        delameta::Result<Wordle::GuessResult> guess(const std::string& guess_word);
    };
}

#ifdef FMT_FORMAT_H_
template<> struct fmt::formatter<Project::Wordle::GuessScore> {
    using Self = Project::Wordle::GuessScore;

    constexpr auto parse(fmt::format_parse_context& ctx) { return ctx.end(); }

    template <typename Ctx> inline auto format(const Self& self, Ctx& ctx) const {
        if (self == Self::GREY) {
            return fmt::format_to(ctx.out(), "GREY");
        } else if (self == Self::YELLOW) {
            return fmt::format_to(ctx.out(), "YELLOW");
        } else if (self == Self::GREEN) {
            return fmt::format_to(ctx.out(), "GREEN");
        }
        return fmt::format_to(ctx.out(), "UNKNOWN");
    }
};
#endif
#endif
