#include "states/game_over.hpp"

#include "states/play.hpp"
#include "states/state_manager.hpp"

#include <cmath>
#include <format>

namespace chessfml::states {

game_over::game_over(sf::RenderWindow& window, const board_t& final_board, winner_t winner)
    : m_window(window), m_renderer(window), m_final_board(final_board), m_winner(winner)
{}

void game_over::init()
{
    m_start_time = std::chrono::steady_clock::now();

    if (m_winner == winner_t::Draw) {
        m_checkmate_text = sf::Text(m_font, "Stalemate!", 64);
    } else {
        m_checkmate_text = sf::Text(m_font, "Checkmate!", 64);
    }

    m_checkmate_text.setFillColor(sf::Color::White);
    sf::FloatRect textBounds = m_checkmate_text.getLocalBounds();
    m_checkmate_text.setPosition({(m_window.getSize().x - textBounds.size.x) / 2, m_window.getSize().y * 0.3f});

    std::string winner_string;
    if (m_winner == winner_t::White) {
        winner_string = "White wins!";
        m_winner_text = sf::Text(m_font, winner_string, 48);
        m_winner_text.setFillColor(sf::Color::White);
    } else if (m_winner == winner_t::Black) {
        winner_string = "Black wins!";
        m_winner_text = sf::Text(m_font, winner_string, 48);
        m_winner_text.setFillColor(sf::Color(150, 150, 150));  // Dark gray for black
    } else if (m_winner == winner_t::Draw) {
        winner_string = "Draw!";
        m_winner_text = sf::Text(m_font, winner_string, 48);
        m_winner_text.setFillColor(sf::Color::White);
    }

    sf::FloatRect winnerBounds = m_winner_text.getLocalBounds();
    m_winner_text.setPosition({(m_window.getSize().x - winnerBounds.size.x) / 2, m_window.getSize().y * 0.5f});
}

void game_over::handle_event(const sf::Event& event)
{
    if (const auto* key_pressed = event.getIf<sf::Event::KeyPressed>()) {
        if (key_pressed->scancode == sf::Keyboard::Scancode::Escape ||
            key_pressed->scancode == sf::Keyboard::Scancode::Enter) {
            m_manager->pop_state();  // GameOverState
            m_manager->pop_state();  // PlayState
            // TODO: check this
            // m_manager->pop_states(2);
        }
    }
}

void game_over::update(float dt)
{
    m_countdown -= dt;

    if (m_countdown <= 0.0f) {
        m_manager->pop_states(2);
    }
}

void game_over::render()
{
    m_renderer.render(m_final_board);

    // Create semi-transparent overlay
    sf::RectangleShape overlay({static_cast<float>(m_window.getSize().x), static_cast<float>(m_window.getSize().y)});
    overlay.setFillColor(sf::Color(0, 0, 0, 180));  // Semi-transparent black
    m_window.draw(overlay);

    int seconds_left = static_cast<int>(std::ceil(m_countdown));
    m_countdown_text = sf::Text(m_font, std::format("Returning to menu in {} seconds...", seconds_left), 24);
    m_countdown_text.setFillColor(sf::Color(200, 200, 200));

    sf::FloatRect countdownBounds = m_countdown_text.getLocalBounds();
    m_countdown_text.setPosition({(m_window.getSize().x - countdownBounds.size.x) / 2, m_window.getSize().y * 0.7f});

    // Draw all elements
    m_window.draw(m_checkmate_text);
    m_window.draw(m_winner_text);
    m_window.draw(m_countdown_text);
}

}  // namespace chessfml::states
