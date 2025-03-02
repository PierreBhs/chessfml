#include "states/state_manager.hpp"

namespace chessfml {

state_manager::state_manager(sf::RenderWindow& window) : m_states(), m_window(window) {}

void state_manager::pop_state()
{
    if (!m_states.empty()) {
        m_states.pop();

        if (!m_states.empty()) {
            m_states.top()->resume();
        }
    }
}

void state_manager::pop_states(int count)
{
    for (int i = 0; i < count && !m_states.empty(); ++i) {
        m_states.pop();
    }

    if (!m_states.empty()) {
        m_states.top()->resume();
    }
}

void state_manager::clear_states()
{
    while (!m_states.empty()) {
        m_states.pop();
    }
}

void state_manager::handle_events()
{
    if (m_states.empty())
        return;

    while (const std::optional event = m_window.pollEvent()) {
        if (event->is<sf::Event::Closed>()) {
            m_window.close();
            return;
        }

        m_states.top()->handle_event(*event);
    }
}

void state_manager::update(float dt)
{
    if (!m_states.empty()) {
        m_states.top()->update(dt);
    }
}

void state_manager::render()
{
    if (!m_states.empty()) {
        m_window.clear(sf::Color(50, 50, 50));  // Dark gray background
        m_states.top()->render();
        m_window.display();
    }
}

}  // namespace chessfml
