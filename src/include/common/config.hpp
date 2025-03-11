#pragma once

#include <SFML/Graphics.hpp>
#include <string_view>

namespace chessfml {

using move_t = int;

}

namespace chessfml::config {

struct game
{
    static constexpr auto WIDTH{1200u};
    static constexpr auto HEIGHT{1200u};

    static constexpr auto CENTER_X{static_cast<float>(WIDTH) / 2.0f};
    static constexpr auto CENTER_Y{static_cast<float>(HEIGHT) / 2.0f};
};

struct board
{
    // Using 90% of the window size to ensure it fits with margins
    static constexpr auto BOARD_PERCENTAGE{0.95f};
    static constexpr auto MIN_DIMENSION{static_cast<float>((game::WIDTH < game::HEIGHT) ? game::WIDTH : game::HEIGHT)};

    static constexpr auto size_ui{MIN_DIMENSION * BOARD_PERCENTAGE};

    static constexpr auto offset_x{(static_cast<float>(game::WIDTH) - size_ui) / 2.0f};
    static constexpr auto offset_y{(static_cast<float>(game::HEIGHT) - size_ui) / 2.0f};

    static constexpr auto tile_size_ui{size_ui / 8.0f};

    static constexpr std::array<sf::Color, 2> tile_colors{sf::Color{240, 217, 181}, sf::Color{181, 136, 99}};

    static constexpr auto size{8};

    static constexpr std::string_view fen_starting_position{"rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1"};
};

}  // namespace chessfml::config
