#pragma once

#include <optional>
#include <string>
#include <string_view>
#include "game/board.hpp"
#include "game/game_state.hpp"

namespace chessfml::fen {

using parse_result = std::optional<std::string>;

[[nodiscard]] parse_result parse_fen(std::string_view fen, board_t& board, game_state& state) noexcept;
[[nodiscard]] bool         validate_board_section(std::string_view fen_board) noexcept;
[[nodiscard]] std::string  create_fen(const board_t& board, const game_state& state);

namespace detail {

[[nodiscard]] std::vector<std::string_view> split(std::string_view str, char delimiter) noexcept;

[[nodiscard]] parse_result parse_board_section(std::string_view section, board_t& board) noexcept;
[[nodiscard]] parse_result parse_active_color(std::string_view section, game_state& state) noexcept;
[[nodiscard]] parse_result parse_castling_rights(std::string_view section, game_state& state) noexcept;
[[nodiscard]] parse_result parse_en_passant(std::string_view section, game_state& state) noexcept;
[[nodiscard]] parse_result parse_halfmove_clock(std::string_view section, game_state& state) noexcept;
[[nodiscard]] parse_result parse_fullmove_number(std::string_view section, game_state& state) noexcept;
}  // namespace detail

}  // namespace chessfml::fen
