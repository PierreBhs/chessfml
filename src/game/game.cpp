#include "game/game.hpp"

#include "common/config.hpp"
#include "states/menu_state.hpp"

namespace chessfml {

game::game()
    : m_window(sf::VideoMode({config::game::WIDTH, config::game::HEIGHT}), "ChesSfmL"), m_state_manager(m_window)
{
    // Initialize with menu state
    m_state_manager.push_state<menu_state>(m_window);
}

void game::run()
{
    sf::Clock clock;

    while (m_window.isOpen() && m_state_manager.running()) {
        m_state_manager.handle_events();

        float dt = clock.restart().asSeconds();
        m_state_manager.update(dt);
        m_state_manager.render();
    }
}

}  // namespace chessfml
