#include "common/fen.hpp"

#include "common/utils.hpp"

#include <cctype>
#include <charconv>
#include <format>
#include <print>
#include <ranges>

namespace chessfml::fen {

parse_result parse_fen(std::string_view fen, board_t& board, game_state& state) noexcept
{
    auto sections = detail::split(fen, ' ');

    if (sections.empty()) {
        return "Invalid FEN format: missing sections";
    }

    if (auto result = detail::parse_board_section(sections[0], board); result) {
        return result;
    }

    if (sections.size() > 1) {
        if (auto result = detail::parse_active_color(sections[1], state); result) {
            return result;
        }
    }

    state.clear_castling_rights();
    if (sections.size() > 2) {
        if (auto result = detail::parse_castling_rights(sections[2], state); result) {
            return result;
        }
    }

    state.set_en_passant_target(std::nullopt);
    if (sections.size() > 3) {
        if (auto result = detail::parse_en_passant(sections[3], state); result) {
            return result;
        }
    }

    state.set_halfmove_clock(0);
    if (sections.size() > 4) {
        if (auto result = detail::parse_halfmove_clock(sections[4], state); result) {
            return result;
        }
    }

    state.set_fullmove_number(1);
    if (sections.size() > 5) {
        if (auto result = detail::parse_fullmove_number(sections[5], state); result) {
            return result;
        }
    }

    return std::nullopt;
}

bool validate_board_section(std::string_view fen_board) noexcept
{
    int rank = 0;
    int file = 0;

    for (char c : fen_board) {
        if (c == '/') {
            if (file != 8) {
                return false;
            }
            rank++;
            file = 0;
        } else if (std::isdigit(c)) {
            int empty_squares = c - '0';
            if (empty_squares < 1 || empty_squares > 8) {
                return false;
            }
            file += empty_squares;
        } else if (std::isalpha(c)) {
            char upper_c = static_cast<char>(std::toupper(c));
            if (upper_c != 'P' && upper_c != 'R' && upper_c != 'N' && upper_c != 'B' && upper_c != 'Q' &&
                upper_c != 'K') {
                return false;
            }
            file++;
        } else {
            return false;
        }

        if (file > 8) {
            return false;
        }
    }

    return rank == 7 && file == 8;
}

std::string create_fen(const board_t& board, const game_state& state)
{
    std::string fen;

    for (int rank = 0; rank < 8; ++rank) {
        int empty_count = 0;

        for (int file = 0; file < 8; ++file) {
            const auto& piece = board[rank * 8 + file];

            if (piece.get_type() == piece_t::type_t::Empty) {
                empty_count++;
            } else {
                if (empty_count > 0) {
                    fen += std::format("{}", empty_count);
                    empty_count = 0;
                }
                fen += piece.c_type();
            }
        }

        if (empty_count > 0) {
            fen += std::format("{}", empty_count);
        }

        if (rank < 7) {
            fen += '/';
        }
    }

    fen += std::format(" {}", (state.get_player_turn() == game_state::player_turn::White ? 'w' : 'b'));

    fen += ' ';
    bool has_castling = false;

    if (state.can_castle_kingside(game_state::player_turn::White)) {
        fen += 'K';
        has_castling = true;
    }
    if (state.can_castle_queenside(game_state::player_turn::White)) {
        fen += 'Q';
        has_castling = true;
    }
    if (state.can_castle_kingside(game_state::player_turn::Black)) {
        fen += 'k';
        has_castling = true;
    }
    if (state.can_castle_queenside(game_state::player_turn::Black)) {
        fen += 'q';
        has_castling = true;
    }

    if (!has_castling) {
        fen += '-';
    }

    fen += ' ';
    if (const auto& ep_target = state.get_en_passant_target(); ep_target) {
        fen += position_to_algebraic(*ep_target);
    } else {
        fen += '-';
    }

    fen += std::format(" {} {}", state.get_halfmove_clock(), state.get_fullmove_number());

    return fen;
}

namespace detail {

std::vector<std::string_view> split(std::string_view str, char delimiter) noexcept
{
    std::vector<std::string_view> result;

    for (const auto& word : str | std::views::split(delimiter)) {
        std::string_view sv(word.data(), word.size());
        if (!sv.empty()) {
            result.push_back(sv);
        }
    }

    return result;
}

parse_result parse_board_section(std::string_view section, board_t& board) noexcept
{
    if (!validate_board_section(section)) {
        return "Invalid board section in FEN";
    }

    auto result = board.fen_to_board(section);
    if (!result) {
        return "Invalid board section in FEN";
    }

    return std::nullopt;
}

parse_result parse_active_color(std::string_view section, game_state& state) noexcept
{
    if (section.size() != 1) {
        return "Invalid active color in FEN (should be 'w' or 'b')";
    }

    switch (section[0]) {
        case 'w':
            state.set_player_turn(game_state::player_turn::White);
            break;
        case 'b':
            state.set_player_turn(game_state::player_turn::Black);
            break;
        default:
            return "Invalid active color in FEN (should be 'w' or 'b')";
    }

    return std::nullopt;
}

parse_result parse_castling_rights(std::string_view section, game_state& state) noexcept
{
    state.clear_castling_rights();

    if (section == "-") {
        // No castling rights
        return std::nullopt;
    }

    for (char c : section) {
        switch (c) {
            case 'K':
                state.enable_kingside_castling(game_state::player_turn::White);
                break;
            case 'Q':
                state.enable_queenside_castling(game_state::player_turn::White);
                break;
            case 'k':
                state.enable_kingside_castling(game_state::player_turn::Black);
                break;
            case 'q':
                state.enable_queenside_castling(game_state::player_turn::Black);
                break;
            default:
                return std::format("Invalid castling rights character in FEN: '{}'", c);
        }
    }

    return std::nullopt;
}

parse_result parse_en_passant(std::string_view section, game_state& state) noexcept
{
    if (section == "-") {
        state.set_en_passant_target(std::nullopt);
        return std::nullopt;
    }

    if (section.size() != 2) {
        return "Invalid en passant target in FEN (should be algebraic notation like 'e3')";
    }

    // Convert algebraic notation to board position
    std::string section_str{section};
    move_t      pos = algebraic_to_position(section_str);
    if (pos == 0xFF) {
        return "Invalid en passant target in FEN (invalid algebraic notation)";
    }

    state.set_en_passant_target(pos);
    return std::nullopt;
}

parse_result parse_halfmove_clock(std::string_view section, game_state& state) noexcept
{
    int halfmove = 0;
    auto [ptr, ec] = std::from_chars(section.data(), section.data() + section.size(), halfmove);

    if (ec != std::errc() || halfmove < 0) {
        return "Invalid halfmove clock in FEN (should be a non-negative integer)";
    }

    state.set_halfmove_clock(halfmove);
    return std::nullopt;
}

parse_result parse_fullmove_number(std::string_view section, game_state& state) noexcept
{
    int fullmove = 0;
    auto [ptr, ec] = std::from_chars(section.data(), section.data() + section.size(), fullmove);

    if (ec != std::errc() || fullmove <= 0) {
        return "Invalid fullmove number in FEN (should be a positive integer)";
    }

    state.set_fullmove_number(fullmove);
    return std::nullopt;
}

}  // namespace detail
}  // namespace chessfml::fen
