#pragma once

#include "game/board.hpp"
#include "game/game_state.hpp"
#include "game/moves.hpp"
#include "states/state.hpp"
#include "ui/board_renderer.hpp"

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

enum class winner_type { None, White, Black };

class play_state : public state
{
public:
    play_state(sf::RenderWindow& window);
    ~play_state() override = default;

    void init() override;
    void handle_event(const sf::Event& event) override;
    void update(float dt) override;
    void render() override;

private:
    void   handle_mouse_click(const sf::Vector2i& pos);
    void   update_selected_tile(move_t clicked_pos);
    void   try_select(move_t pos);
    void   try_switch_selection(move_t new_pos);
    void   clear_selection();
    bool   is_valid_selection(move_t pos) const;
    bool   is_valid_move(move_t to) const;
    bool   move_piece(move_t from, move_t to);
    move_t convert_to_board_pos(const sf::Vector2i& pos) const;
    void   check_for_checkmate();

    // Handle special moves
    void handle_en_passant(move_t from, move_t to);
    void handle_castling(move_t from, move_t to);
    void update_game_state_after_move(move_t from, move_t to);
    void move_piece_board(move_t from, move_t to);

    sf::RenderWindow& m_window;
    board_renderer    m_renderer;

    board_t                m_board;
    game_state             m_game_state;
    selection_system       m_selection;
    std::vector<move_info> m_current_valid_moves;
};

}  // namespace chessfml
