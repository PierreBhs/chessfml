#include "game/game.hpp"

#include "config/config.hpp"
#include "game/piece.hpp"

#include <SFML/Window/Keyboard.hpp>
#include <SFML/Window/Mouse.hpp>
#include <chrono>
#include <print>
#include <thread>

namespace {

auto calculate_selected_tile(int file, int rank)
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

void game::handle_mouse_click(const sf::Vector2i& pos)
{
    const sf::Vector2u windowSize = m_window.getSize();
    const int          squareWidth = windowSize.x / config::board::size;
    const int          squareHeight = windowSize.y / config::board::size;

    const int file = pos.x / squareWidth;
    const int rank = (config::board::size - 1) - (pos.y / squareHeight);

    std::println("CLICKED ON {} {}", file, rank);

    if (file < 0 || file >= config::board::size || rank < 0 || rank >= config::board::size)
        return;

    set_selected_tile(file, rank);
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
}

void game::set_selected_tile(int file, int rank)
{
    const auto pos = calculate_selected_tile(file, rank);

    if (m_selected_tile == -1) {
        if (m_board[pos].get_type() != piece_t::type_t::Empty) {
            m_selected_tile = pos;
            m_renderer.set_selected_tile(pos);
        }
        return;
    }

    const auto  prev_pos = m_selected_tile;
    const auto& selected_piece = m_board[prev_pos];
    auto        move_list = get_valid_moves(selected_piece);

    if (std::ranges::find(move_list, pos) != move_list.end()) {
        std::println("Piece prev: ");
        selected_piece.print_piece();
        move_piece(prev_pos, pos);
        std::println("Piece prev: ");
        selected_piece.print_piece();

        m_board.print_board();
    }

    m_selected_tile = -1;
    m_renderer.set_selected_tile(-1);
}
}  // namespace chessfml
