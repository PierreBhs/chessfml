#pragma once

#include "game/board.hpp"
#include "game/game_state.hpp"
#include "game/moves.hpp"
#include "ui/main_menu.hpp"
#include "ui/render.hpp"

#include <expected>
#include <vector>

namespace chessfml {

class selection_system
{
public:
    void   select(move_t pos) noexcept { m_selected = pos; }
    void   clear() noexcept { m_selected.reset(); }
    bool   has_selection() const noexcept { return m_selected.has_value(); }
    move_t get() const noexcept { return m_selected.value_or(0xFF); }

private:
    std::optional<move_t> m_selected;
};

class game
{
public:
    game();
    void run();
    void process_events();
    void update(double elapsed);
    bool move_piece(move_t from, move_t to);
    void update_selected_tile(move_t clicked_pos);

private:
    void process_game_events(const sf::Event& event);
    void handle_mouse_click(const sf::Vector2i& mouse_pos);
    void try_select(move_t pos);
    void try_switch_selection(move_t new_pos);
    bool is_valid_selection(move_t pos) const;
    void clear_selection();

    move_t convert_to_board_pos(const sf::Vector2i& mouse_pos) const noexcept;
    bool   is_valid_move(move_t to) const;

    void handle_en_passant(move_t, move_t);
    void handle_castling(move_t, move_t);
    void update_game_state_after_move(move_t, move_t);

    void move_piece_board(move_t, move_t);

    // SFML components
    sf::RenderWindow m_window;
    renderer         m_renderer;

    // Game components
    board_t          m_board;
    game_state       m_state{};
    selection_system m_selection{};

    std::vector<move_info> m_current_valid_moves;

    // App state management
    AppState m_app_state{AppState::MainMenu};
    MainMenu m_main_menu;
};

}  // namespace chessfml
