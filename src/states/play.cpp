#include "states/play.hpp"

#include "common/config.hpp"
#include "common/utils.hpp"
#include "states/game_over.hpp"
#include "states/state_manager.hpp"

#include <SFML/Window/Keyboard.hpp>
#include <SFML/Window/Mouse.hpp>

#include <algorithm>
#include <chrono>
#include <random>
#include <thread>

namespace {

std::uint8_t calculate_selected_tile(int file, int rank)
{
    return std::abs(7 - rank) * chessfml::config::board::size + file;
}

}  // namespace

namespace chessfml::states {

play::play(sf::RenderWindow& window, player_t white_player, player_t black_player)
    : m_window(window),
      m_renderer(window),
      m_board(),
      m_game_state(),
      m_white_player(white_player),
      m_black_player(black_player)
{}

play::play(sf::RenderWindow& window,
           const board_t&    board,
           const game_state& state,
           player_t          white_player,
           player_t          black_player)
    : m_window(window),
      m_renderer(window),
      m_board(board),
      m_game_state(state),
      m_white_player(white_player),
      m_black_player(black_player)
{}

void play::init()
{
    bool is_board_empty = std::all_of(
        m_board.begin(), m_board.end(), [](const auto& piece) { return piece.get_type() == piece_t::type_t::Empty; });

    if (is_board_empty) {
        m_board.set_board_fen(config::board::fen_starting_position);
    }

    m_game_state.set_check(move_generator::is_in_check(m_board, m_game_state.get_player_turn()));

    // Lock the board if AI plays as white (needs to make first move)
    m_board_locked = m_game_state.get_player_turn() == game_state::player_turn::White && m_white_player == player_t::AI;
    m_waiting_for_ai_move = m_board_locked;
}

void play::handle_event(const sf::Event& event)
{
    if (const auto* key_pressed = event.getIf<sf::Event::KeyPressed>()) {
        if (key_pressed->scancode == sf::Keyboard::Scancode::Escape) {
            m_manager->pop_state();  // Return to menu
            return;
        }
    }

    // Skip other input handling if board is locked (AI's turn)
    if (m_board_locked) {
        return;
    }

    if (const auto* mouse_button_pressed = event.getIf<sf::Event::MouseButtonPressed>()) {
        if (mouse_button_pressed->button == sf::Mouse::Button::Left) {
            handle_mouse_click({mouse_button_pressed->position.x, mouse_button_pressed->position.y});
        }
    }
}

void play::update(float dt)
{
    if (is_current_player_ai()) {
        handle_ai_turn(dt);
    } else {
        m_waiting_for_ai_move = false;
        m_board_locked = false;
    }

    std::this_thread::sleep_for(std::chrono::milliseconds{30});
}

void play::render()
{
    m_renderer.render(m_board);

    // Make singleton Font ? Too many sf::Font object created for the same font
    static const sf::Font font{"data/fonts/Montserrat-Regular.ttf"};

    sf::Text turn_indicator{font};
    turn_indicator.setFont(font);
    turn_indicator.setCharacterSize(24);
    turn_indicator.setPosition({20, 20});

    const auto current_player = m_game_state.get_player_turn();
    const bool is_ai = is_current_player_ai();

    std::string turn_text = std::string(current_player == game_state::player_turn::White ? "White" : "Black") +
                            "'s turn (" + (is_ai ? "AI" : "Human") + ")";

    turn_indicator.setString(turn_text);
    turn_indicator.setFillColor(sf::Color::White);

    sf::Text shadow = turn_indicator;
    shadow.setFillColor(sf::Color::Black);
    shadow.setPosition(turn_indicator.getPosition() + sf::Vector2f(2, 2));

    m_window.draw(shadow);
    m_window.draw(turn_indicator);

    if (m_waiting_for_ai_move) {
        sf::Text thinking_text{font};
        thinking_text.setCharacterSize(20);
        thinking_text.setString("AI is thinking...");
        thinking_text.setFillColor(sf::Color(220, 220, 100));  // Yellow-ish
        turn_indicator.setPosition({20, 20});

        m_window.draw(thinking_text);
    }
}

void play::handle_ai_turn(float dt)
{
    m_board_locked = true;

    // Add a delay before AI moves to make it more natural
    if (m_waiting_for_ai_move) {
        m_ai_move_timer += dt;
        if (m_ai_move_timer >= m_ai_move_delay) {
            m_ai_move_timer = 0.0f;
            m_waiting_for_ai_move = false;

            try {
                auto ai_move_opt = calculate_ai_move();
                if (ai_move_opt) {
                    bool move_result = execute_move(*ai_move_opt);

                    if (!move_result) {
                        std::println("AI move failed: from {} to {}", ai_move_opt->from, ai_move_opt->to);
                    }
                }
            } catch (const std::exception& e) {
                std::println("Error during AI move: {}", e.what());
            }
        }
    } else {
        m_waiting_for_ai_move = true;
        m_ai_move_timer = 0.0f;
    }
}

std::optional<move_info> play::calculate_ai_move()
{
    std::vector<move_info> all_legal_moves;
    const auto             player_turn = m_game_state.get_player_turn();
    const auto             piece_color =
        (player_turn == game_state::player_turn::White) ? piece_t::color_t::White : piece_t::color_t::Black;

    clear_selection();

    for (move_t pos = 0; pos < 64; ++pos) {
        const auto& piece = m_board[pos];
        if (piece.get_type() != piece_t::type_t::Empty && piece.get_color() == piece_color) {
            auto moves = move_generator::get_legal_moves(m_board, m_game_state, pos);
            all_legal_moves.insert(all_legal_moves.end(), moves.begin(), moves.end());
        }
    }

    if (all_legal_moves.empty()) {
        // No legal moves available (should be detected as checkmate/stalemate elsewhere)
        return std::nullopt;
    }

    static std::random_device                  rd;
    static std::mt19937                        gen(rd());
    std::uniform_int_distribution<std::size_t> dist(0, all_legal_moves.size() - 1);
    std::size_t                                randomIndex = dist(gen);

    return all_legal_moves[randomIndex];
}

bool play::is_current_player_ai() const
{
    const auto current_player = m_game_state.get_player_turn();
    return (current_player == game_state::player_turn::White && m_white_player == player_t::AI) ||
           (current_player == game_state::player_turn::Black && m_black_player == player_t::AI);
}

void play::handle_mouse_click(const sf::Vector2i& mouse_pos)
{
    const auto clicked_pos = convert_to_board_pos(mouse_pos);
    if (clicked_pos != 0xFF) {  // Valid board position
        update_selected_tile(clicked_pos);
    }
}

void play::update_selected_tile(move_t clicked_pos)
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

    auto it =
        std::ranges::find_if(m_current_valid_moves, [clicked_pos](const auto& move) { return move.to == clicked_pos; });

    if (it != m_current_valid_moves.end()) {
        execute_move(*it);
        clear_selection();
    } else {
        try_switch_selection(clicked_pos);
    }
}

void play::try_select(move_t pos)
{
    if (is_valid_selection(pos)) {
        m_selection.select(pos);
        m_renderer.set_selected_tile(pos);

        m_current_valid_moves = move_generator::get_legal_moves(m_board, m_game_state, pos);
        m_renderer.set_valid_moves(m_current_valid_moves);
    }
}

void play::try_switch_selection(move_t new_pos)
{
    if (is_valid_selection(new_pos)) {
        m_selection.select(new_pos);
        m_renderer.set_selected_tile(new_pos);

        m_current_valid_moves = move_generator::get_legal_moves(m_board, m_game_state, new_pos);
        m_renderer.set_valid_moves(m_current_valid_moves);
    } else {
        clear_selection();
    }
}

void play::clear_selection()
{
    m_selection.clear();
    m_renderer.set_selected_tile(-1);
    m_current_valid_moves.clear();
    m_renderer.set_valid_moves({});
}

bool play::is_valid_selection(move_t pos) const
{
    return pos < 64 && m_board[pos].get_type() != piece_t::type_t::Empty &&
           std::to_underlying(m_game_state.get_player_turn()) == std::to_underlying(m_board[pos].get_color());
}

move_t play::convert_to_board_pos(const sf::Vector2i& mouse_pos) const
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

bool play::execute_move(const move_info& move)
{
    if (move.is_en_passant()) {
        handle_en_passant(move.from, move.to);
    }

    if (move.is_castling()) {
        handle_castling(move.from, move.to);
    }

    if (move.is_promotion()) {
        m_board[move.from].set_type(static_cast<piece_t::type_t>(move.promotion_piece));
    }

    move_piece_board(move.from, move.to);
    update_game_state_after_move(move.from, move.to);

    return true;
}

void play::move_piece_board(move_t from, move_t to)
{
    m_board[to] = m_board[from];
    m_board[to].set_pos(to);
    m_board[to].set_moved(true);
    m_board[from] = piece_t{};
}

void play::handle_en_passant(move_t from, move_t to)
{
    const auto [capture_rank, capture_file] = position_to_rank_file(to);
    const auto   piece_color = m_board[from].get_color();
    const int    ep_rank = piece_color == piece_t::color_t::White ? capture_rank + 1 : capture_rank - 1;
    const move_t ep_pos = rank_file_to_position(ep_rank, capture_file);

    m_board[ep_pos] = piece_t{};
}

void play::handle_castling(move_t from, move_t to)
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

void play::update_game_state_after_move(move_t from, move_t to)
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

void play::check_for_game_over()
{
    if (move_generator::is_checkmate(m_board, m_game_state)) {
        winner_t winner = winner_t::None;

        if (m_game_state.get_player_turn() == game_state::player_turn::White) {
            winner = winner_t::Black;
        } else {
            winner = winner_t::White;
        }

        m_manager->push_state<game_over>(m_window, m_board, winner);
    } else if (move_generator::is_stalemate(m_board, m_game_state)) {
        // Stalemate is a draw
        m_manager->push_state<game_over>(m_window, m_board, winner_t::Draw);
    }
}

}  // namespace chessfml::states
