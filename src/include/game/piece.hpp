#pragma once

#include <cstdint>
#include <print>

namespace chessfml {

class piece_t
{
public:
    using pos_t = std::uint8_t;

    enum class type_t { Empty = 0, Pawn, Rook, Knight, Bishop, Queen, King };
    enum class color_t { White = 0, Black };

    piece_t() noexcept = default;
    piece_t(type_t t, pos_t pos, color_t c = color_t::White) : m_type(t), m_pos(pos), m_color(c) {}
    // std::string to_algebraic_notation() const { return "a1"; }

    piece_t(const piece_t&) = default;
    piece_t(piece_t&&) = default;
    piece_t& operator=(piece_t&) = default;
    piece_t& operator=(piece_t&&) = default;

    char c_type() const noexcept
    {
        switch (m_type) {
            case type_t::Pawn:
                return m_color == color_t::White ? 'P' : 'p';
            case type_t::Rook:
                return m_color == color_t::White ? 'R' : 'r';
            case type_t::Knight:
                return m_color == color_t::White ? 'N' : 'n';
            case type_t::Bishop:
                return m_color == color_t::White ? 'B' : 'b';
            case type_t::Queen:
                return m_color == color_t::White ? 'Q' : 'q';
            case type_t::King:
                return m_color == color_t::White ? 'K' : 'k';
            default:
                return '.';
        }
    }

    const type_t&  get_type() const noexcept { return m_type; }
    const pos_t&   get_pos() const noexcept { return m_pos; }
    const color_t& get_color() const noexcept { return m_color; }
    bool           has_moved() const noexcept { return m_has_moved; }

    void set_type(type_t type) noexcept { m_type = type; }
    void set_pos(pos_t pos) noexcept { m_pos = pos; }
    void set_color(color_t color) noexcept { m_color = color; }
    void set_moved(bool moved) noexcept { m_has_moved = moved; }

    void print() const { std::println("type: {} pos: {}", c_type(), get_pos()); }

private:
    type_t  m_type;
    pos_t   m_pos;
    color_t m_color;
    bool    m_has_moved{false};
};

}  // namespace chessfml
