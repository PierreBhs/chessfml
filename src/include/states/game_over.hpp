#pragma once

#include "game/board.hpp"
#include "states/state.hpp"
#include "ui/board_renderer.hpp"

#include <chrono>

namespace chessfml::states {

enum class winner_t;  // Forward declaration

class game_over : public state
{
public:
    game_over(sf::RenderWindow& window, const board_t& final_board, winner_t winner);
    ~game_over() override = default;

    void init() override;
    void handle_event(const sf::Event& event) override;
    void update(float dt) override;
    void render() override;

private:
    sf::RenderWindow&                                  m_window;
    board_renderer                                     m_renderer;
    board_t                                            m_final_board;
    winner_t                                           m_winner;
    std::chrono::time_point<std::chrono::steady_clock> m_start_time;
    float                                              m_countdown{3.0f};

    inline static const sf::Font m_font{"data/fonts/Montserrat-Regular.ttf"};

    sf::Text m_checkmate_text{m_font, "Checkmate!", 64};
    sf::Text m_winner_text{m_font};
    sf::Text m_countdown_text{m_font};
};

}  // namespace chessfml::states
