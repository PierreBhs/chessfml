#pragma once

#include <SFML/Graphics.hpp>
#include <string_view>

namespace chessfml::config {

struct game
{
    static constexpr auto WIDTH{1280u};
    static constexpr auto HEIGHT{1280u};
    static constexpr auto CENTER_WIDTH{static_cast<float>(WIDTH) / 2};
};

struct board
{
    static constexpr auto size_ui{static_cast<float>(game::WIDTH)};
    static constexpr auto offset_ui{0.f};
    // static constexpr auto offset_ui{static_cast<float>(game::WIDTH) / 8};
    static constexpr auto tile_size_ui{size_ui / 8.f};

    static constexpr std::array<sf::Color, 2> tile_colors{sf::Color{240, 217, 181}, sf::Color{181, 136, 99}};

    static constexpr auto size{8};

    static constexpr std::string_view fen_starting_position{"rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR"};
};

}  // namespace chessfml::config
