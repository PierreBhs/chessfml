#pragma once

#include "game/board.hpp"
#include "game/game_state.hpp"
#include "game/moves.hpp"
#include "states/state.hpp"
#include "ui/board_renderer.hpp"

#include <vector>

namespace chessfml {

enum class player_t { Human, AI };

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

enum class winner_t { None, White, Black, Draw };

class play_state : public state
{
public:
    play_state(sf::RenderWindow& window,
               player_t          white_player = player_t ::Human,
               player_t          black_player = player_t ::Human);
    // loading a game from FEN
    play_state(sf::RenderWindow& window,
               const board_t&    board,
               const game_state& state,
               player_t          white_player = player_t ::Human,
               player_t          black_player = player_t ::Human);

    ~play_state() override = default;

    void init() override;
    void handle_event(const sf::Event& event) override;
    void update(float dt) override;
    void render() override;

private:
    // Human input handling
    void   handle_mouse_click(const sf::Vector2i& pos);
    void   update_selected_tile(move_t clicked_pos);
    void   try_select(move_t pos);
    void   try_switch_selection(move_t new_pos);
    void   clear_selection();
    bool   is_valid_selection(move_t pos) const;
    move_t convert_to_board_pos(const sf::Vector2i& pos) const;

    // Move execution
    bool execute_move(const move_info& move);
    void move_piece_board(move_t from, move_t to);
    void handle_en_passant(move_t from, move_t to);
    void handle_castling(move_t from, move_t to);
    void update_game_state_after_move(move_t from, move_t to);
    void check_for_game_over();

    // Ai related
    bool                     is_current_player_ai() const;
    void                     handle_ai_turn(float dt);
    std::optional<move_info> calculate_ai_move();

    sf::RenderWindow& m_window;
    board_renderer    m_renderer;

    board_t                m_board;
    game_state             m_game_state;
    selection_system       m_selection;
    std::vector<move_info> m_current_valid_moves;

    player_t m_white_player{player_t ::Human};
    player_t m_black_player{player_t ::Human};

    float m_ai_move_timer{0.0f};
    float m_ai_move_delay{1.0f};  // 1 second delay between AI moves, to make it feel less robotic
    bool  m_waiting_for_ai_move{false};
    bool  m_board_locked{false};  // When true, user inputs affecting the board are ignored
};

}  // namespace chessfml
