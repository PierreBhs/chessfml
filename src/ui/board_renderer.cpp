#include "ui/board_renderer.hpp"

#include "config/config.hpp"
#include "game/piece.hpp"

#include <SFML/Graphics/CircleShape.hpp>
#include <SFML/Graphics/Color.hpp>
#include <filesystem>
#include <print>

namespace {

size_t texture_index(chessfml::piece_t::type_t type, chessfml::piece_t::color_t c)
{
    const size_t color_offset = c == chessfml::piece_t::color_t::White ? 6 : 0;
    return color_offset + static_cast<size_t>(type) - 1;
}

sf::Vector2f tile_index_to_sfml_pos(std::uint8_t index)
{
    const unsigned x = index % chessfml::config::board::size;
    const unsigned y = index / chessfml::config::board::size;

    const float tile_size = chessfml::config::board::tile_size_ui;
    return {x * tile_size, y * tile_size};
}

}  // namespace

namespace chessfml {

board_renderer::board_renderer(sf::RenderWindow& window)
    : renderer(window),
      m_drawing_board(),
      m_pieces_texture(),
      m_selected_tile(-1),
      m_glow_shader(std::filesystem::path{"data/glow.frag"}, sf::Shader::Type::Fragment)
{
    init_board();
    load_pieces_textures();
    m_glow_shader.setUniform("texture", sf::Shader::CurrentTexture);
    m_glow_shader.setUniform("glowColor", sf::Glsl::Vec4(1.0f, 0.9f, 0.2f, 0.7f));
}

void board_renderer::render(const board_t& board)
{
    draw_board();
    draw_pieces(board);
    draw_possible_moves();
}

void board_renderer::init_board()
{
    using config = config::board;
    auto y_pos{config::offset_ui}, x_pos{config::offset_ui};

    for (auto y{0u}; y < 8u; ++y, y_pos += config::tile_size_ui) {
        for (auto x{0u}; x < 8u; ++x, x_pos += config::tile_size_ui) {
            auto& tile = m_drawing_board[y * 8 + x];
            tile.setPosition({x_pos, y_pos});
            tile.setSize({config::tile_size_ui, config::tile_size_ui});
            tile.setFillColor(config::board::tile_colors[(x + y) % 2]);
        }
        x_pos = config::offset_ui;
    }
}

void board_renderer::draw_board()
{
    for (const auto& tile : m_drawing_board) {
        m_window.draw(tile);
    }
}

void board_renderer::draw_pieces(const board_t& board)
{
    for (const auto& piece : board) {
        if (piece.get_type() == piece_t::type_t::Empty) {
            continue;
        }

        sf::Sprite sprite{m_pieces_texture[texture_index(piece.get_type(), piece.get_color())]};
        sprite.setScale({0.075f, 0.075f});
        sprite.setPosition(tile_index_to_sfml_pos(piece.get_pos()));

        // glow if selected
        if (piece.get_pos() == m_selected_tile) {
            sf::Sprite glow_sprite = sprite;
            create_glow_effect(glow_sprite);
        }

        m_window.draw(sprite);
    }
}

void board_renderer::draw_possible_moves()
{
    if (m_selected_tile == -1) {
        return;
    }

    auto tile_size = config::board::tile_size_ui;
    for (const auto& move : m_valid_moves) {
        sf::CircleShape circle(tile_size / 5);

        // Use different colors for different move types
        if (move.is_capture()) {
            circle.setFillColor(sf::Color{200, 0, 0, 120});  // Red for captures
        } else if (move.is_castling()) {
            circle.setFillColor(sf::Color{0, 0, 200, 120});  // Blue for castling
        } else {
            circle.setFillColor(sf::Color{0, 0, 0, 100});  // Default
        }

        auto [pos_x, pos_y] = tile_index_to_sfml_pos(move.to);
        circle.setPosition({pos_x + tile_size / 4, pos_y + tile_size / 4});

        m_window.draw(circle);
    }
}

void board_renderer::load_pieces_textures()
{
    const std::array<std::string, 12> filenames = {"bP.png",
                                                   "bR.png",
                                                   "bN.png",
                                                   "bB.png",
                                                   "bQ.png",
                                                   "bK.png",
                                                   "wP.png",
                                                   "wR.png",
                                                   "wN.png",
                                                   "wB.png",
                                                   "wQ.png",
                                                   "wK.png"};

    for (size_t i = 0; i < m_pieces_texture.size(); ++i) {
        if (!m_pieces_texture[i].loadFromFile("data/" + filenames[i])) {
            // Handle error (e.g., throw exception or log message)
            // throw std::runtime_error("Failed to load texture: " + filenames[i]);
        }
    }
}

void board_renderer::create_glow_effect(sf::Sprite& sprite)
{
    const float base_scale = 0.075f;
    const float glow_scale = base_scale * 1.2f;  // Slightly larger for glow effect

    sprite.setScale({glow_scale, glow_scale});
    sprite.setColor(sf::Color(255, 255, 255, 75));  // Semi-transparent
    sprite.move({-14.f, -14.f});

    m_window.draw(sprite, &m_glow_shader);
}

}  // namespace chessfml
