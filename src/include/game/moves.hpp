#pragma once
#include <vector>
#include "config/config.hpp"
#include "game/board.hpp"
#include "game/game_state.hpp"

namespace chessfml {

struct move_info
{
    move_t       from;
    move_t       to;
    bool         is_capture{false};
    bool         is_promotion{false};
    bool         is_en_passant{false};
    bool         is_castling{false};
    std::uint8_t promotion_piece{0};  // Corresponds to piece_t::type_t

    constexpr bool operator==(const move_info& other) const noexcept { return from == other.from && to == other.to; }
};

class move_generator
{
public:
    static std::vector<move_info> get_pseudo_legal_moves(const board_t& board, const game_state& state, move_t pos);
    static std::vector<move_info> get_legal_moves(const board_t& board, const game_state& state, move_t pos);
    static bool                   is_legal_move(const board_t& board, const game_state& state, move_t from, move_t to);

    static bool is_in_check(const board_t& board, game_state::player_turn player);

private:
    static std::vector<move_info> get_pawn_moves(const board_t& board, const game_state& state, move_t pos);
    static std::vector<move_info> get_rook_moves(const board_t& board, move_t pos);
    static std::vector<move_info> get_knight_moves(const board_t& board, move_t pos);
    static std::vector<move_info> get_bishop_moves(const board_t& board, move_t pos);
    static std::vector<move_info> get_queen_moves(const board_t& board, move_t pos);
    static std::vector<move_info> get_king_moves(const board_t& board, const game_state& state, move_t pos);

    static bool is_square_attacked(const board_t& board, move_t square, game_state::player_turn attacker);
};

}  // namespace chessfml
