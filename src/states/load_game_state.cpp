#include "states/load_game_state.hpp"

#include <format>
#include "states/state_manager.hpp"

namespace chessfml {

load_game_state::load_game_state(sf::RenderWindow& window) : m_window(window) {}

void load_game_state::init()
{
    m_start_time = std::chrono::steady_clock::now();

    // Setup message text
    m_message.setFillColor(sf::Color::White);

    // Center the text
    sf::FloatRect textBounds = m_message.getLocalBounds();
    m_message.setPosition({(m_window.getSize().x - textBounds.size.x) / 2,
                           (m_window.getSize().y - textBounds.size.y) / 2 - textBounds.position.y});
}

void load_game_state::handle_event(const sf::Event& event)
{
    if (const auto* key_pressed = event.getIf<sf::Event::KeyPressed>()) {
        if (key_pressed->scancode == sf::Keyboard::Scancode::Escape ||
            key_pressed->scancode == sf::Keyboard::Scancode::Enter) {
            m_manager->pop_state();  // Return to menu immediately on key press
        }
    }
}

void load_game_state::update([[maybe_unused]] float dt)
{
    auto current_time = std::chrono::steady_clock::now();
    auto elapsed = std::chrono::duration_cast<std::chrono::seconds>(current_time - m_start_time).count();

    if (elapsed >= m_display_time) {
        m_manager->pop_state();  // Return to menu after delay
    }
}

void load_game_state::render()
{
    m_window.clear(sf::Color(50, 50, 50));  // Dark gray background

    // Calculate remaining time
    auto current_time = std::chrono::steady_clock::now();
    auto elapsed = std::chrono::duration_cast<std::chrono::seconds>(current_time - m_start_time).count();
    int  remaining = static_cast<int>(m_display_time - elapsed);

    // Draw message
    m_window.draw(m_message);

    // Draw timer
    if (remaining > 0) {
        sf::Text timer(m_font, std::format("Returning to menu in {} seconds...", remaining), 24);
        timer.setFillColor(sf::Color(200, 200, 200));

        sf::FloatRect timerBounds = timer.getLocalBounds();
        timer.setPosition({(m_window.getSize().x - timerBounds.size.x) / 2, m_window.getSize().y * 0.7f});

        m_window.draw(timer);
    }
}

}  // namespace chessfml
