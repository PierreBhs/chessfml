#include "states/play_state.hpp"

#include "common/config.hpp"
#include "common/utils.hpp"
#include "states/game_over.hpp"
#include "states/state_manager.hpp"

#include <SFML/Window/Keyboard.hpp>
#include <SFML/Window/Mouse.hpp>
#include <algorithm>
#include <chrono>
#include <thread>

namespace {

std::uint8_t calculate_selected_tile(int file, int rank)
{
    return std::abs(7 - rank) * chessfml::config::board::size + file;
}

}  // namespace

namespace chessfml {

play_state::play_state(sf::RenderWindow& window) : m_window(window), m_renderer(window) {}

void play_state::init()
{
    m_board.set_board_fen(config::board::fen_starting_position);
}

void play_state::handle_event(const sf::Event& event)
{
    if (const auto* key_pressed = event.getIf<sf::Event::KeyPressed>()) {
        if (key_pressed->scancode == sf::Keyboard::Scancode::Escape) {
            m_manager->pop_state();  // Return to menu
        }
    } else if (const auto* mouse_button_pressed = event.getIf<sf::Event::MouseButtonPressed>()) {
        if (mouse_button_pressed->button == sf::Mouse::Button::Left) {
            handle_mouse_click({mouse_button_pressed->position.x, mouse_button_pressed->position.y});
        }
    }
}

void play_state::update([[maybe_unused]] float dt)
{
    // Nothing to update in regular gameplay, just a slight delay for smoother animation
    std::this_thread::sleep_for(std::chrono::milliseconds{30});
}

void play_state::render()
{
    m_renderer.render(m_board);
}

void play_state::handle_mouse_click(const sf::Vector2i& mouse_pos)
{
    const auto clicked_pos = convert_to_board_pos(mouse_pos);
    update_selected_tile(clicked_pos);
}

void play_state::update_selected_tile(move_t clicked_pos)
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

void play_state::try_select(move_t pos)
{
    if (is_valid_selection(pos)) {
        m_selection.select(pos);
        m_renderer.set_selected_tile(pos);

        // Get and display valid moves
        m_current_valid_moves = move_generator::get_legal_moves(m_board, m_game_state, pos);
        m_renderer.set_valid_moves(m_current_valid_moves);
    }
}

void play_state::try_switch_selection(move_t new_pos)
{
    if (is_valid_selection(new_pos)) {
        m_selection.select(new_pos);
        m_renderer.set_selected_tile(new_pos);

        // Update valid moves for new selection
        m_current_valid_moves = move_generator::get_legal_moves(m_board, m_game_state, new_pos);
        m_renderer.set_valid_moves(m_current_valid_moves);
    } else {
        clear_selection();
    }
}

void play_state::clear_selection()
{
    m_selection.clear();
    m_renderer.set_selected_tile(-1);
    m_current_valid_moves.clear();
    m_renderer.set_valid_moves({});
}

bool play_state::is_valid_selection(move_t pos) const
{
    return pos < 64 && m_board[pos].get_type() != piece_t::type_t::Empty &&
           std::to_underlying(m_game_state.get_player_turn()) == std::to_underlying(m_board[pos].get_color());
}

bool play_state::is_valid_move(move_t to) const
{
    return std::ranges::any_of(m_current_valid_moves, [to](const auto& move) { return move.to == to; });
}

move_t play_state::convert_to_board_pos(const sf::Vector2i& mouse_pos) const
{
    int relative_x = mouse_pos.x - static_cast<int>(config::board::offset_x);
    int relative_y = mouse_pos.y - static_cast<int>(config::board::offset_y);

    if (relative_x < 0 || relative_y < 0 || relative_x >= static_cast<int>(config::board::size_ui) ||
        relative_y >= static_cast<int>(config::board::size_ui)) {
        return 0xFF;  // Invalid position
    }

    const int file = relative_x / static_cast<int>(config::board::tile_size_ui);
    const int rank = (config::board::size - 1) - (relative_y / static_cast<int>(config::board::tile_size_ui));

    return calculate_selected_tile(file, rank);
}

bool play_state::move_piece(move_t from, move_t to)
{
    if (from == to)
        return false;

    auto it = std::ranges::find_if(m_current_valid_moves, [to](const auto& move) { return move.to == to; });

    if (it == m_current_valid_moves.end())
        return false;

    const auto& move = *it;

    if (move.is_en_passant()) {
        handle_en_passant(from, to);
    }

    if (move.is_castling()) {
        handle_castling(from, to);
    }

    if (move.is_promotion()) {
        m_board[from].set_type(static_cast<piece_t::type_t>(move.promotion_piece));
    }

    move_piece_board(from, to);

    update_game_state_after_move(from, to);

    // Debug print
    m_board.print();

    return true;
}

void play_state::move_piece_board(move_t from, move_t to)
{
    m_board[to] = m_board[from];
    m_board[to].set_pos(to);
    m_board[to].set_moved(true);
    m_board[from] = piece_t{};
}

void play_state::handle_en_passant(move_t from, move_t to)
{
    const auto [capture_rank, capture_file] = position_to_rank_file(to);
    const auto   piece_color = m_board[from].get_color();
    const int    ep_rank = piece_color == piece_t::color_t::White ? capture_rank + 1 : capture_rank - 1;
    const move_t ep_pos = rank_file_to_position(ep_rank, capture_file);

    m_board[ep_pos] = piece_t{};
}

void play_state::handle_castling(move_t from, move_t to)
{
    const auto piece_color = m_board[from].get_color();
    const bool is_kingside = to > from;

    if (is_kingside) {
        // Kingside castling: rook moves from h1/h8 to f1/f8
        const move_t rook_from = (piece_color == piece_t::color_t::White) ? 7 : 63;
        const move_t rook_to = (piece_color == piece_t::color_t::White) ? 5 : 61;

        move_piece_board(rook_from, rook_to);
    } else {
        // Queenside castling: rook moves from a1/a8 to d1/d8
        const move_t rook_from = (piece_color == piece_t::color_t::White) ? 0 : 56;
        const move_t rook_to = (piece_color == piece_t::color_t::White) ? 3 : 59;

        move_piece_board(rook_from, rook_to);
    }
}

void play_state::update_game_state_after_move(move_t from, move_t to)
{
    if (m_board[to].get_type() == piece_t::type_t::Pawn &&
        std::abs(static_cast<int>(to) - static_cast<int>(from)) == 16) {
        // Pawn moved two squares, set en passant target
        m_game_state.set_en_passant_target((from + to) / 2);
    } else {
        m_game_state.set_en_passant_target(std::nullopt);
    }

    if (m_board[to].get_type() == piece_t::type_t::King) {
        m_game_state.disable_kingside_castling(m_game_state.get_player_turn());
        m_game_state.disable_queenside_castling(m_game_state.get_player_turn());
    } else if (m_board[to].get_type() == piece_t::type_t::Rook) {
        if (from == (m_game_state.get_player_turn() == game_state::player_turn::White ? 0 : 56)) {
            m_game_state.disable_queenside_castling(m_game_state.get_player_turn());
        } else if (from == (m_game_state.get_player_turn() == game_state::player_turn::White ? 7 : 63)) {
            m_game_state.disable_kingside_castling(m_game_state.get_player_turn());
        }
    }

    m_game_state.next_turn();

    m_game_state.set_check(move_generator::is_in_check(m_board, m_game_state.get_player_turn()));

    check_for_game_over();
}

void play_state::check_for_game_over()
{
    if (move_generator::is_checkmate(m_board, m_game_state)) {
        winner_t winner = winner_t::None;

        if (m_game_state.get_player_turn() == game_state::player_turn::White) {
            winner = winner_t::Black;
        } else {
            winner = winner_t::White;
        }

        m_manager->push_state<game_over_state>(m_window, m_board, winner);
    } else if (move_generator::is_stalemate(m_board, m_game_state)) {
        // Stalemate is a draw
        m_manager->push_state<game_over_state>(m_window, m_board, winner_t::Draw);
    }
}

}  // namespace chessfml
