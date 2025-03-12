#pragma once

#include <SFML/Window/Event.hpp>

namespace chessfml::states {

class state_manager;

class state
{
public:
    virtual ~state() = default;

    virtual void init() = 0;
    virtual void handle_event(const sf::Event& event) = 0;
    virtual void update(float dt) = 0;
    virtual void render() = 0;

    virtual void pause() {}
    virtual void resume() {}

    void set_manager(state_manager* manager) { m_manager = manager; }

protected:
    state_manager* m_manager{nullptr};
};

}  // namespace chessfml::states
