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

sf::Vector2f tile_to_position(std::uint8_t index)
{
    using namespace chessfml::config;
    const unsigned x = index % board::size;
    const unsigned y = index / board::size;
    return {board::offset_x + x * board::tile_size_ui, board::offset_y + y * board::tile_size_ui};
}

sf::Vector2f tile_center(std::uint8_t index)
{
    using namespace chessfml::config;
    sf::Vector2f pos = tile_to_position(index);
    return {pos.x + board::tile_size_ui / 2.0f, pos.y + board::tile_size_ui / 2.0f};
}

float calculate_sprite_scale(float sprite_width, float target_percentage)
{
    return (chessfml::config::board::tile_size_ui * target_percentage) / sprite_width;
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
    using namespace config;
    float y_pos{board::offset_y};

    for (auto y{0u}; y < 8u; ++y, y_pos += board::tile_size_ui) {
        float x_pos{board::offset_x};
        for (auto x{0u}; x < 8u; ++x, x_pos += board::tile_size_ui) {
            auto& tile = m_drawing_board[y * 8 + x];
            tile.setPosition({x_pos, y_pos});
            tile.setSize({board::tile_size_ui, board::tile_size_ui});
            tile.setFillColor(board::tile_colors[(x + y) % 2]);
        }
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

        // Get texture dimensions
        const float texture_width = static_cast<float>(sprite.getTexture().getSize().x);
        const float texture_height = static_cast<float>(sprite.getTexture().getSize().y);

        // Calculate scale - use 85% of tile size for better visibility
        const float scale_factor = calculate_sprite_scale(texture_width, 0.85f);

        sprite.setScale({scale_factor, scale_factor});

        // Calculate the scaled dimensions of the piece
        const float scaled_width = texture_width * scale_factor;
        const float scaled_height = texture_height * scale_factor;

        // Get the top-left corner of the tile
        sf::Vector2f tile_pos = tile_to_position(piece.get_pos());

        // Calculate offsets to center the piece in the tile
        const float x_offset = (config::board::tile_size_ui - scaled_width) / 2.0f;
        const float y_offset = (config::board::tile_size_ui - scaled_height) / 2.0f;

        // Set position with centering offsets
        sprite.setPosition({tile_pos.x + x_offset, tile_pos.y + y_offset});

        // Glow if selected
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

    for (const auto& move : m_valid_moves) {
        // Get the center of the tile for positioning the circle
        sf::Vector2f center = tile_center(move.to);

        // Size the circle proportionally to the tile
        float           circle_radius = config::board::tile_size_ui / 5.0f;
        sf::CircleShape circle(circle_radius);

        // Center the circle on the tile
        circle.setOrigin({circle_radius, circle_radius});
        circle.setPosition(center);

        // Use different colors for different move types
        if (move.is_capture()) {
            circle.setFillColor(sf::Color{200, 0, 0, 120});  // Red for captures
        } else if (move.is_castling()) {
            circle.setFillColor(sf::Color{0, 0, 200, 120});  // Blue for castling
        } else {
            circle.setFillColor(sf::Color{0, 0, 0, 100});  // Default
        }

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
    // Get original position and scale
    sf::Vector2f original_position = sprite.getPosition();
    sf::Vector2f original_scale = sprite.getScale();

    // Scale up slightly for glow effect
    sprite.setScale({original_scale.x * 1.2f, original_scale.y * 1.2f});

    // Adjust position to keep the piece centered with the new scale
    float offset_x = (sprite.getGlobalBounds().size.x - original_scale.x * sprite.getTexture().getSize().x) / 2.0f;
    float offset_y = (sprite.getGlobalBounds().size.y - original_scale.y * sprite.getTexture().getSize().y) / 2.0f;

    sprite.setPosition({original_position.x - offset_x / 2.0f, original_position.y - offset_y / 2.0f});
    sprite.setColor(sf::Color(255, 255, 255, 75));  // Semi-transparent

    m_window.draw(sprite, &m_glow_shader);
}

}  // namespace chessfml
