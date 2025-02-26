#pragma once

#include "game/board.hpp"

#include <SFML/Graphics.hpp>
#include <array>

namespace chessfml {

class renderer
{
public:
    renderer(sf::RenderWindow&);

    void render(const board&);
    void set_selected_tile(int tile);

private:
    void init_board();
    void load_pieces_textures();

    void draw_board();
    void draw_pieces(const board&);
    void draw_possible_moves(const board& board);

    void create_glow_effect(sf::Sprite&);

    sf::RenderWindow& m_window;

    std::array<sf::RectangleShape, 64> m_drawing_board;
    std::array<sf::Texture, 12>        m_pieces_texture;
    int                                m_selected_tile{-1};

    sf::Shader m_glow_shader;
};

}  // namespace chessfml
