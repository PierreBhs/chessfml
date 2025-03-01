#include "game/game.hpp"

#include "config/config.hpp"
#include "config/utils.hpp"
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

void game::update_selected_tile(move_t clicked_pos)
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

    if (is_valid_move(clicked_pos)) {
        move_piece(current_sel, clicked_pos);
        clear_selection();
    } else {
        try_switch_selection(clicked_pos);
    }
}

void game::try_select(move_t pos)
{
    if (is_valid_selection(pos)) {
        m_selection.select(pos);
        m_renderer.set_selected_tile(pos);

        // Get and display valid moves
        m_current_valid_moves = move_generator::get_legal_moves(m_board, m_state, pos);
        m_renderer.set_valid_moves(m_current_valid_moves);
    }
}

void game::try_switch_selection(move_t new_pos)
{
    if (is_valid_selection(new_pos)) {
        m_selection.select(new_pos);
        m_renderer.set_selected_tile(new_pos);

        // Update valid moves for new selection
        m_current_valid_moves = move_generator::get_legal_moves(m_board, m_state, new_pos);
        m_renderer.set_valid_moves(m_current_valid_moves);
    } else {
        clear_selection();
    }
}

void game::clear_selection()
{
    m_selection.clear();
    m_renderer.set_selected_tile(-1);
    m_current_valid_moves.clear();
    m_renderer.set_valid_moves({});
}

bool game::is_valid_selection(move_t pos) const
{
    return pos < 64 && m_board[pos].get_type() != piece_t::type_t::Empty &&
           std::to_underlying(m_state.get_player_turn()) == std::to_underlying(m_board[pos].get_color());
}

bool game::is_valid_move(move_t to) const
{
    return std::ranges::any_of(m_current_valid_moves, [to](const auto& move) { return move.to == to; });
}

bool game::move_piece(move_t from, move_t to)
{
    if (from == to)
        return false;

    // Find the move info if it exists
    auto it = std::ranges::find_if(m_current_valid_moves, [to](const auto& move) { return move.to == to; });

    if (it == m_current_valid_moves.end())
        return false;

    const auto& move = *it;

    if (move.is_en_passant) {
        const auto [capture_rank, capture_file] = position_to_rank_file(move.to);
        const auto   piece_color = m_board[from].get_color();
        const int    ep_rank = piece_color == piece_t::color_t::White ? capture_rank + 1 : capture_rank - 1;
        const move_t ep_pos = rank_file_to_position(ep_rank, capture_file);

        m_board[ep_pos] = piece_t{};  // Empty the captured pawn's square
    }

    if (move.is_castling) {
        const auto piece_color = m_board[from].get_color();
        const bool is_kingside = to > from;

        if (is_kingside) {
            const move_t rook_from = piece_color == piece_t::color_t::White ? 7 : 63;
            const move_t rook_to = piece_color == piece_t::color_t::White ? 5 : 61;

            m_board[rook_to] = m_board[rook_from];
            m_board[rook_to].set_pos(rook_to);
            m_board[rook_to].set_moved(true);
            m_board[rook_from] = piece_t{};
        } else {
            const move_t rook_from = piece_color == piece_t::color_t::White ? 0 : 56;
            const move_t rook_to = piece_color == piece_t::color_t::White ? 3 : 59;

            m_board[rook_to] = m_board[rook_from];
            m_board[rook_to].set_pos(rook_to);
            m_board[rook_to].set_moved(true);
            m_board[rook_from] = piece_t{};
        }
    }

    if (move.is_promotion) {
        m_board[from].set_type(static_cast<piece_t::type_t>(move.promotion_piece));
    }

    // Standard move
    m_board[to] = m_board[from];
    m_board[to].set_pos(to);
    m_board[to].set_moved(true);
    m_board[from] = piece_t{};

    // Update en passant target
    if (m_board[to].get_type() == piece_t::type_t::Pawn &&
        std::abs(static_cast<int>(to) - static_cast<int>(from)) == 16) {
        // Pawn moved two squares, set en passant target
        m_state.set_en_passant_target((from + to) / 2);
    } else {
        m_state.set_en_passant_target(std::nullopt);
    }

    // Update castling rights
    if (m_board[to].get_type() == piece_t::type_t::King) {
        m_state.disable_kingside_castling(m_state.get_player_turn());
        m_state.disable_queenside_castling(m_state.get_player_turn());
    } else if (m_board[to].get_type() == piece_t::type_t::Rook) {
        if (from == (m_state.get_player_turn() == game_state::player_turn::White ? 0 : 56)) {
            m_state.disable_queenside_castling(m_state.get_player_turn());
        } else if (from == (m_state.get_player_turn() == game_state::player_turn::White ? 7 : 63)) {
            m_state.disable_kingside_castling(m_state.get_player_turn());
        }
    }

    m_state.next_turn();

    m_state.set_check(move_generator::is_in_check(m_board, m_state.get_player_turn()));

    m_board.print_board();

    return true;
}

move_t game::convert_to_board_pos(const sf::Vector2i& mouse_pos) const noexcept
{
    constexpr int      board_size = config::board::size;
    const sf::Vector2u window_size = m_window.getSize();
    const int          tile_width = window_size.x / board_size;
    const int          tile_height = window_size.y / board_size;

    const int file = mouse_pos.x / tile_width;
    const int rank = (config::board::size - 1) - (mouse_pos.y / tile_height);

    return calculate_selected_tile(file, rank);
}

}  // namespace chessfml
