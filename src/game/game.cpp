#include "game/game.hpp"

#include "config/config.hpp"
#include "game/piece.hpp"

#include <SFML/Window/Keyboard.hpp>
#include <SFML/Window/Mouse.hpp>
#include <algorithm>
#include <chrono>
#include <cstdint>
#include <print>
#include <thread>

namespace {

std::uint8_t calculate_selected_tile(int file, int rank)
{
    return std::abs(7 - rank) * chessfml::config::board::size + file;
}

}  // namespace

namespace chessfml {

game::game()
    : m_window(sf::VideoMode({config::game::WIDTH, config::game::HEIGHT}), "ChesSfmL"), m_renderer(m_window), m_board()
{
    // m_window.setFramerateLimit(30);
    m_board.set_board_fen(config::board::fen_starting_position);
}

void game::run()
{
    while (m_window.isOpen()) {
        process_events();
        update(0.);
        m_renderer.render(m_board);
        std::println("Went through loop!");
    }
}

void game::process_events()
{
    while (const std::optional event = m_window.pollEvent()) {
        if (event->is<sf::Event::Closed>()) {
            m_window.close();
        } else if (const auto* key_pressed = event->getIf<sf::Event::KeyPressed>()) {
            if (key_pressed->scancode == sf::Keyboard::Scancode::Escape) {
                m_window.close();
            }
        } else if (const auto* mouse_button_pressed = event->getIf<sf::Event::MouseButtonPressed>()) {
            if (mouse_button_pressed->button == sf::Mouse::Button::Left) {
                handle_mouse_click({mouse_button_pressed->position.x, mouse_button_pressed->position.y});
            }
        }
    }
}

void game::update([[maybe_unused]] double elapsed)
{
    std::this_thread::sleep_for(std::chrono::milliseconds{50});
}

void game::handle_mouse_click(const sf::Vector2i& mouse_pos)
{
    const auto clicked_pos = convert_to_board_pos(mouse_pos);
    update_selected_tile(clicked_pos);
}

void game::update_selected_tile(std::uint8_t clicked_pos)
{

    if (!m_selection.has_selection()) {
        try_select(clicked_pos);
        return;
    }

    const auto current_sel = m_selection.get();

    if (clicked_pos == current_sel) {
        clear_selection();
        return;
    }

    if (is_valid_move(current_sel, clicked_pos)) {
        move_piece(current_sel, clicked_pos);
        clear_selection();
    } else {
        try_switch_selection(clicked_pos);
    }
}

void game::move_piece(std::uint8_t from, std::uint8_t to)
{
    if (from == to)
        return;

    // Capture piece if moving to occupied square
    if (m_board[to].get_type() != piece_t::type_t::Empty) {
        // Handle captured piece (could add to captured list)
    }

    // Move the piece
    m_board[to] = m_board[from];
    m_board[to].set_pos(static_cast<piece_t::pos_t>(to));
    m_board[to].set_moved(true);

    m_board[from] = piece_t{};
    m_board[from].set_pos(from);

    m_state.next_turn();
}

void game::try_select(std::uint8_t pos)
{
    if (is_valid_selection(pos)) {
        m_selection.select(pos);
        m_renderer.set_selected_tile(pos);
    }
}

void game::try_switch_selection(std::uint8_t new_pos)
{
    if (is_valid_selection(new_pos)) {
        m_selection.select(new_pos);
        m_renderer.set_selected_tile(new_pos);
    } else {
        clear_selection();
    }
}

bool game::is_valid_selection(std::uint8_t pos) const
{
    return pos < 64 && m_board[pos].get_type() != piece_t::type_t::Empty &&
           std::to_underlying(m_state.get_player_turn()) == std::to_underlying(m_board[pos].get_color());
}

void game::clear_selection()
{
    m_selection.clear();
    m_renderer.set_selected_tile(-1);
}

std::uint8_t game::convert_to_board_pos(const sf::Vector2i& mouse_pos) const noexcept
{
    constexpr int      board_size = config::board::size;
    const sf::Vector2u window_size = m_window.getSize();
    const int          tile_width = window_size.x / board_size;
    const int          tile_height = window_size.y / board_size;

    const int file = mouse_pos.x / tile_width;
    const int rank = (config::board::size - 1) - (mouse_pos.y / tile_height);

    // if (file < 0 || file >= config::board::size || rank < 0 || rank >= config::board::size)
    //     return;

    return calculate_selected_tile(file, rank);
}

bool game::is_valid_move(std::uint8_t from, std::uint8_t to) const
{
    const auto& piece = m_board[from];
    auto        moves = get_valid_moves(piece);
    return std::ranges::contains(moves, to);
}

}  // namespace chessfml
