#pragma once

#include "states/state.hpp"

#include <SFML/Graphics/RenderWindow.hpp>

#include <memory>
#include <stack>

namespace chessfml {

class state_manager
{
public:
    state_manager(sf::RenderWindow& window);
    ~state_manager() = default;

    template <typename T, typename... Args>
    void push_state(Args&&... args);
    template <typename T, typename... Args>
    void replace_state(Args&&... args);

    void pop_state();
    void pop_states(int count);
    void clear_states();

    void handle_events();
    void update(float dt);
    void render();

    sf::RenderWindow& get_window() { return m_window; }
    bool              running() const { return !m_states.empty(); }

private:
    std::stack<std::unique_ptr<state>> m_states;
    sf::RenderWindow&                  m_window;
};

template <typename T, typename... Args>
void state_manager::push_state(Args&&... args)
{
    auto state = std::make_unique<T>(std::forward<Args>(args)...);
    state->set_manager(this);
    state->init();

    if (!m_states.empty()) {
        m_states.top()->pause();
    }

    m_states.push(std::move(state));
}

template <typename T, typename... Args>
void state_manager::replace_state(Args&&... args)
{
    auto new_state = std::make_unique<T>(std::forward<Args>(args)...);
    new_state->set_manager(this);

    if (!m_states.empty()) {
        m_states.pop();
    }

    new_state->init();
    m_states.push(std::move(new_state));
}

}  // namespace chessfml
