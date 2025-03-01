#pragma once

#include "game/board.hpp"
#include "game/game_state.hpp"
#include "ui/render.hpp"

#include <SFML/Graphics/RenderWindow.hpp>

#include <cstdint>
#include <expected>

namespace chessfml {

class selection_system
{
public:
    void select(std::uint8_t pos) noexcept { m_selected = pos; }

    void clear() noexcept { m_selected.reset(); }

    bool has_selection() const noexcept { return m_selected.has_value(); }

    std::uint8_t get() const noexcept { return m_selected.value_or(-1); }

private:
    std::optional<std::uint8_t> m_selected;
};

class game
{
public:
    game();

    void run();
    void process_events();
    void update(double elapsed);

    void move_piece(std::uint8_t, std::uint8_t);
    void update_selected_tile(std::uint8_t);

private:
    void handle_mouse_click(const sf::Vector2i&);

    void try_select(std::uint8_t);
    void try_switch_selection(std::uint8_t);
    bool is_valid_selection(std::uint8_t) const;
    void clear_selection();

    std::uint8_t convert_to_board_pos(const sf::Vector2i&) const noexcept;
    bool         is_valid_move(std::uint8_t from, std::uint8_t to) const;

    // SFML stuff
    sf::RenderWindow m_window;
    renderer         m_renderer;

    // Game stuff
    board            m_board;
    int              m_selected_tile{-1};
    game_state       m_state{};
    selection_system m_selection{};
};

}  // namespace chessfml
