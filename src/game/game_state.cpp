#include "game/game_state.hpp"

namespace chessfml {

// Castling flags
namespace {
constexpr std::uint8_t WHITE_KINGSIDE_FLAG = 0x01;
constexpr std::uint8_t WHITE_QUEENSIDE_FLAG = 0x02;
constexpr std::uint8_t BLACK_KINGSIDE_FLAG = 0x04;
constexpr std::uint8_t BLACK_QUEENSIDE_FLAG = 0x08;
}  // namespace

bool game_state::can_castle_kingside(player_turn player) const
{
    const auto flag = (player == player_turn::White) ? WHITE_KINGSIDE_FLAG : BLACK_KINGSIDE_FLAG;
    return (m_castling_rights & flag) != 0;
}

bool game_state::can_castle_queenside(player_turn player) const
{
    const auto flag = (player == player_turn::White) ? WHITE_QUEENSIDE_FLAG : BLACK_QUEENSIDE_FLAG;
    return (m_castling_rights & flag) != 0;
}

void game_state::disable_kingside_castling(player_turn player)
{
    const auto flag = (player == player_turn::White) ? WHITE_KINGSIDE_FLAG : BLACK_KINGSIDE_FLAG;
    m_castling_rights &= ~flag;
}

void game_state::disable_queenside_castling(player_turn player)
{
    const auto flag = (player == player_turn::White) ? WHITE_QUEENSIDE_FLAG : BLACK_QUEENSIDE_FLAG;
    m_castling_rights &= ~flag;
}

void game_state::enable_kingside_castling(player_turn player) noexcept
{
    const auto flag = (player == player_turn::White) ? WHITE_KINGSIDE_FLAG : BLACK_KINGSIDE_FLAG;
    m_castling_rights |= flag;
}

void game_state::enable_queenside_castling(player_turn player) noexcept
{
    const auto flag = (player == player_turn::White) ? WHITE_QUEENSIDE_FLAG : BLACK_QUEENSIDE_FLAG;
    m_castling_rights |= flag;
}

void game_state::update_move_counters(bool is_capture_or_pawn_move) noexcept
{
    m_halfmove_clock = is_capture_or_pawn_move ? 0 : m_halfmove_clock + 1;

    if (m_turn == player_turn::Black) {
        m_fullmove_number++;
    }
}

}  // namespace chessfml
