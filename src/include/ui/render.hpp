#pragma once

#include "game/board.hpp"
#include "game/moves.hpp"

#include <SFML/Graphics.hpp>

#include <array>
#include <vector>

namespace chessfml {

class renderer
{
public:
    renderer(sf::RenderWindow&);
    void render(const board_t&);
    void set_selected_tile(int tile);
    void set_valid_moves(const std::vector<move_info>& moves);

private:
    void init_board();
    void load_pieces_textures();
    void draw_board();
    void draw_pieces(const board_t&);
    void draw_possible_moves();
    void create_glow_effect(sf::Sprite&);

    sf::RenderWindow&                  m_window;
    std::array<sf::RectangleShape, 64> m_drawing_board;
    std::array<sf::Texture, 12>        m_pieces_texture;
    int                                m_selected_tile{-1};
    sf::Shader                         m_glow_shader;
    std::vector<move_info>             m_valid_moves;
};

}  // namespace chessfml
