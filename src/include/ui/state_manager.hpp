#pragma once

#include <SFML/Graphics/RenderWindow.hpp>

#include <memory>
#include <stack>

namespace chessfml {

class game_state;

class state_manager
{
public:
    state_manager() = default;
    ~state_manager() = default;

    void push_state(std::unique_ptr<game_state> state);
    void pop_state();
    void change_state(std::unique_ptr<game_state> state);

    game_state* current_state();
    bool        is_empty() const;

private:
    std::stack<std::unique_ptr<game_state>> m_states;
};

class game_state
{
public:
    game_state(state_manager& state_mgr, sf::RenderWindow& window) : m_state_manager(state_mgr), m_window(window) {}

    virtual ~game_state() = default;

    virtual void init() = 0;
    virtual void handle_input() = 0;
    virtual void update(float dt) = 0;
    virtual void render() = 0;

    virtual void pause() {}
    virtual void resume() {}

protected:
    state_manager&    m_state_manager;
    sf::RenderWindow& m_window;
};

}  // namespace chessfml
