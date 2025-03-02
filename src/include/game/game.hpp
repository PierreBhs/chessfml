#pragma once

#include <SFML/Graphics.hpp>
#include "states/state_manager.hpp"

namespace chessfml {

class game
{
public:
    game();
    ~game() = default;

    void run();

private:
    sf::RenderWindow m_window;
    state_manager    m_state_manager;
};

}  // namespace chessfml
