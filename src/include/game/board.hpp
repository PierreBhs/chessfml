#pragma once

#include "game/piece.hpp"

#include <array>
#include <expected>

namespace chessfml {

enum class fen_error { InvalidCharacter, InvalidFormat };
class board_t
{
public:
    std::expected<std::string_view, fen_error> fen_to_board(std::string_view);
    void                                       set_board_fen(std::string_view fen);
    void                                       print() const;

    auto begin() const { return m_board.begin(); }
    auto end() const { return m_board.end(); }
    // auto cbegin() const { return m_board.cbegin(); }
    // auto cend() const { return m_board.cend(); }

    template <typename Self>
    auto&& operator[](this Self&& self, std::size_t idx)
    {
        return std::forward<Self>(self).m_board[idx];
    }

private:
    std::array<piece_t, 64> m_board;
};

}  // namespace chessfml
