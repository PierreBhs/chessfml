#pragma once

#include <cstdint>
#include <print>
#include <vector>

#include "config/config.hpp"
/*

Base definition of a piece

*/

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

    char c_type() const
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

    const type_t&  get_type() const { return m_type; }
    const pos_t&   get_pos() const { return m_pos; }
    const color_t& get_color() const { return m_color; }
    bool           has_moved() const { return m_has_moved; }

    void set_type(type_t type) { m_type = type; }
    void set_pos(pos_t pos) { m_pos = pos; }
    void set_color(color_t color) { m_color = color; }
    void set_moved(bool moved) { m_has_moved = moved; }

    void print() const { std::println("type: {} pos: {}", c_type(), get_pos()); }

private:
    type_t  m_type;
    pos_t   m_pos;
    color_t m_color;
    bool    m_has_moved{false};
};

// class empty : public piece<empty>
// {
// public:
//     using piece::piece;
//     empty(std::uint8_t pos) : piece(pos, color::White) {}
//     char name() const { return '.'; }
// };

// class pawn : public piece<pawn>
// {
// public:
//     using piece::piece;
//     char name() const { return (m_color == color::White) ? 'P' : 'p'; }
// };

// class rook : public piece<rook>
// {
// public:
//     using piece::piece;
//     char name() const { return (m_color == color::White) ? 'R' : 'r'; }
// };

// class knight : public piece<knight>
// {
// public:
//     using piece::piece;
//     char name() const { return (m_color == color::White) ? 'N' : 'n'; }
// };

// class bishop : public piece<bishop>
// {
// public:
//     using piece::piece;
//     char name() const { return (m_color == color::White) ? 'B' : 'b'; }
// };

// class queen : public piece<queen>
// {
// public:
//     using piece::piece;
//     char name() const { return (m_color == color::White) ? 'Q' : 'q'; }
// };

// class king : public piece<king>
// {
// public:
//     using piece::piece;
//     char name() const { return (m_color == color::White) ? 'K' : 'k'; }
// };

// using piece_t = std::variant<empty, pawn, rook, knight, bishop, queen, king>;
// enum class piece_type { Empty = 0, Pawn, Rook, Knight, Bishop, Queen, King };

// // Type trait to map piece types to enum
// template <typename T>
// struct piece_trait
// {};

// template <>
// struct piece_trait<empty>
// {
//     static constexpr piece_type type = piece_type::Empty;
// };

// template <>
// struct piece_trait<pawn>
// {
//     static constexpr piece_type type = piece_type::Pawn;
// };

// template <>
// struct piece_trait<rook>
// {
//     static constexpr piece_type type = piece_type::Rook;
// };

// template <>
// struct piece_trait<knight>
// {
//     static constexpr piece_type type = piece_type::Knight;
// };

// template <>
// struct piece_trait<bishop>
// {
//     static constexpr piece_type type = piece_type::Bishop;
// };

// template <>
// struct piece_trait<queen>
// {
//     static constexpr piece_type type = piece_type::Queen;
// };

// template <>
// struct piece_trait<king>
// {
//     static constexpr piece_type type = piece_type::King;
// };

// struct piece_visitor
// {
//     template <typename T>
//     auto operator()(const T& piece) const -> std::tuple<piece_type, std::uint8_t, color>
//     {
//         return {piece_trait<T>::type, piece.m_pos, piece.m_color};
//     }
// };

// struct piece_visitor_name
// {
//     template <typename T>
//     auto operator()(const T& piece) const -> char
//     {
//         return piece.name();
//     }
// };

}  // namespace chessfml
