#pragma once

#include <utility>
#include "config/config.hpp"

namespace chessfml {

constexpr std::pair<int, int> position_to_rank_file(move_t pos)
{
    return {pos / config::board::size, pos % config::board::size};
}

constexpr move_t rank_file_to_position(int rank, int file)
{
    return rank * config::board::size + file;
}

constexpr bool is_valid_position(move_t pos)
{
    return pos < config::board::size * config::board::size;
}

inline std::string position_to_algebraic(move_t pos)
{
    const auto [rank, file] = position_to_rank_file(pos);
    return std::string{static_cast<char>('a' + file), static_cast<char>('8' - rank)};
}

inline move_t algebraic_to_position(const std::string& algebraic)
{
    if (algebraic.size() != 2)
        return 0xFF;

    const char file_char = algebraic[0];
    const char rank_char = algebraic[1];

    if (file_char < 'a' || file_char > 'h' || rank_char < '1' || rank_char > '8') {
        return 0xFF;
    }

    const int file = file_char - 'a';
    const int rank = '8' - rank_char;

    return rank_file_to_position(rank, file);
}

}  // namespace chessfml
