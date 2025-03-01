#pragma once
#include <vector>
#include "config/config.hpp"
#include "game/board.hpp"
#include "game/game_state.hpp"

namespace chessfml {

enum class move_type_flag : std::uint8_t {
    Normal = 0,
    Capture = 1 << 0,    // 0x01
    EnPassant = 1 << 1,  // 0x02
    Castling = 1 << 2,   // 0x04
    Promotion = 1 << 3   // 0x08
};

constexpr move_type_flag operator|(move_type_flag a, move_type_flag b)
{
    return static_cast<move_type_flag>(static_cast<std::uint8_t>(a) | static_cast<std::uint8_t>(b));
}

constexpr move_type_flag operator&(move_type_flag a, move_type_flag b)
{
    return static_cast<move_type_flag>(static_cast<std::uint8_t>(a) & static_cast<std::uint8_t>(b));
}

constexpr bool has_flag(move_type_flag value, move_type_flag flag)
{
    return (static_cast<std::uint8_t>(value) & static_cast<std::uint8_t>(flag)) != 0;
}

struct move_info
{
    move_t         from;
    move_t         to;
    move_type_flag type{move_type_flag ::Normal};
    std::uint8_t   promotion_piece{0};  // Corresponds to piece_t::type_t

    constexpr bool operator==(const move_info& other) const noexcept { return from == other.from && to == other.to; }

    bool is_capture() const { return has_flag(type, move_type_flag ::Capture); }
    bool is_en_passant() const { return has_flag(type, move_type_flag ::EnPassant); }
    bool is_castling() const { return has_flag(type, move_type_flag ::Castling); }
    bool is_promotion() const { return has_flag(type, move_type_flag ::Promotion); }
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
