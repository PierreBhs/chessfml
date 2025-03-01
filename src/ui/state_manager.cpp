#include "ui/state_manager.hpp"

namespace chessfml {

void state_manager::push_state(std::unique_ptr<game_state> state)
{
    if (!m_states.empty()) {
        m_states.top()->pause();
    }

    m_states.push(std::move(state));
    m_states.top()->init();
}

void state_manager::pop_state()
{
    if (!m_states.empty()) {
        m_states.pop();
    }

    if (!m_states.empty()) {
        m_states.top()->resume();
    }
}

void state_manager::change_state(std::unique_ptr<game_state> state)
{
    if (!m_states.empty()) {
        m_states.pop();
    }

    m_states.push(std::move(state));
    m_states.top()->init();
}

game_state* state_manager::current_state()
{
    return m_states.empty() ? nullptr : m_states.top().get();
}

bool state_manager::is_empty() const
{
    return m_states.empty();
}

}  // namespace chessfml
