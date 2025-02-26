#pragma once

#include "game/board.hpp"
#include "ui/render.hpp"

#include <SFML/Graphics/RenderWindow.hpp>

#include <expected>

namespace chessfml {

class game
{
public:
    game();

    void run();
    void process_events();
    void update(double elapsed);

    void move_piece(std::uint8_t, std::uint8_t);
    void set_selected_tile(int file, int rank);

private:
    void handle_mouse_click(const sf::Vector2i&);

    sf::RenderWindow m_window;
    renderer         m_renderer;

    board m_board;
    int   m_selected_tile{-1};
};

}  // namespace chessfml
