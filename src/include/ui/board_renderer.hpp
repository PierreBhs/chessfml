#pragma once

#include <array>
#include <vector>
#include "game/board.hpp"
#include "game/moves.hpp"
#include "ui/renderer.hpp"

namespace chessfml {

class board_renderer : public renderer
{
public:
    explicit board_renderer(sf::RenderWindow& window);
    ~board_renderer() override = default;

    void render() override {}  // Not used - we use the overloaded version below
    void render(const board_t& board);

    void set_selected_tile(int tile) { m_selected_tile = tile; }
    void set_valid_moves(const std::vector<move_info>& moves) { m_valid_moves = moves; }

private:
    void init_board();
    void load_pieces_textures();
    void draw_board();
    void draw_pieces(const board_t& board);
    void draw_possible_moves();
    void create_glow_effect(sf::Sprite& sprite);

    std::array<sf::RectangleShape, 64> m_drawing_board;
    std::array<sf::Texture, 12>        m_pieces_texture;
    int                                m_selected_tile{-1};
    sf::Shader                         m_glow_shader;
    std::vector<move_info>             m_valid_moves;
};

}  // namespace chessfml
