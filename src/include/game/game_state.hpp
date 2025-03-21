// In "game/game_state.hpp"
#pragma once

#include <cstdint>
#include <optional>

#include "common/config.hpp"

namespace chessfml {

class game_state
{
public:
    enum class player_turn { White, Black };

    game_state() = default;

    const player_turn& get_player_turn() const { return m_turn; }
    void               set_player_turn(player_turn turn) noexcept { m_turn = turn; }

    void next_turn() { m_turn = (m_turn == player_turn::White) ? player_turn::Black : player_turn::White; }

    std::optional<move_t> get_en_passant_target() const { return m_en_passant_target; }
    void                  set_en_passant_target(std::optional<move_t> target) { m_en_passant_target = target; }

    bool can_castle_kingside(player_turn player) const;
    bool can_castle_queenside(player_turn player) const;
    void disable_kingside_castling(player_turn player);
    void disable_queenside_castling(player_turn player);
    void enable_kingside_castling(player_turn player) noexcept;
    void enable_queenside_castling(player_turn player) noexcept;
    void clear_castling_rights() noexcept { m_castling_rights = 0x00; }

    bool is_check() const { return m_in_check; }
    void set_check(bool in_check) { m_in_check = in_check; }

    [[nodiscard]] int get_halfmove_clock() const noexcept { return m_halfmove_clock; }
    void              set_halfmove_clock(int clock) noexcept { m_halfmove_clock = clock; }

    [[nodiscard]] int get_fullmove_number() const noexcept { return m_fullmove_number; }
    void              set_fullmove_number(int number) noexcept { m_fullmove_number = number; }

    // Update halfmove clock and fullmove number when making a move
    void update_move_counters(bool is_capture_or_pawn_move) noexcept;

private:
    player_turn           m_turn{player_turn::White};
    std::optional<move_t> m_en_passant_target{std::nullopt};

    // Castling rights (bit flags: 0=white kingside, 1=white queenside, 2=black kingside, 3=black queenside)
    std::uint8_t m_castling_rights{0x0F};

    bool m_in_check{false};
    int  m_halfmove_clock{0};   // Number of halfmoves since last capture or pawn advance
    int  m_fullmove_number{1};  // Number of full moves, starts at 1 and increments after Black's move
};

}  // namespace chessfml
