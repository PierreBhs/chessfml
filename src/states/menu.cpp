#include "states/menu.hpp"

#include "states/game_selection.hpp"
#include "states/load_game.hpp"
#include "states/state_manager.hpp"

#include <thread>

namespace chessfml::states {

menu::menu(sf::RenderWindow& window) : m_window(window), m_renderer(window) {}

void menu::init() {}

void menu::handle_event(const sf::Event& event)
{
    if (const auto* key_pressed = event.getIf<sf::Event::KeyPressed>()) {
        if (key_pressed->scancode == sf::Keyboard::Scancode::Escape) {
            m_window.close();
        } else if (key_pressed->scancode == sf::Keyboard::Scancode::Up) {
            m_selected_option =
                (m_selected_option - 1 + static_cast<int>(menu_option::Count)) % static_cast<int>(menu_option::Count);
            m_renderer.set_selected_option(m_selected_option);
        } else if (key_pressed->scancode == sf::Keyboard::Scancode::Down) {
            m_selected_option = (m_selected_option + 1) % static_cast<int>(menu_option::Count);
            m_renderer.set_selected_option(m_selected_option);
        } else if (key_pressed->scancode == sf::Keyboard::Scancode::Enter) {
            activate_selected_option();
        }

    } else if (const auto* mouse_moved = event.getIf<sf::Event::MouseMoved>()) {
        // Handle mouse movement for highlighting buttons
        sf::Vector2f mouse_pos = sf::Vector2f(mouse_moved->position);

        for (int i = 0; i < static_cast<int>(menu_option::Count); ++i) {
            bool is_hovered = m_renderer.is_button_hovered(i, mouse_pos);
            m_renderer.set_hovered(i, is_hovered);
        }
    } else if (const auto* mouse_pressed = event.getIf<sf::Event::MouseButtonPressed>()) {
        if (mouse_pressed->button == sf::Mouse::Button::Left) {
            sf::Vector2f mouse_pos = sf::Vector2f(mouse_pressed->position);

            for (int i = 0; i < static_cast<int>(menu_option::Count); ++i) {
                if (m_renderer.is_button_hovered(i, mouse_pos)) {
                    m_selected_option = i;
                    m_renderer.set_selected_option(i);
                    activate_selected_option();
                    break;
                }
            }
        }
    }
}

void menu::activate_selected_option()
{
    switch (static_cast<menu_option>(m_selected_option)) {
        case menu_option::Play:
            m_manager->push_state<game_selection>(m_window);
            break;
        case menu_option::LoadGame:
            m_manager->push_state<load_game>(m_window);
            break;
        case menu_option::Exit:
            m_window.close();
            break;
        default:
            break;
    }
}
void menu::update([[maybe_unused]] float dt)
{
    std::this_thread::sleep_for(std::chrono::milliseconds{30});
}

void menu::render()
{
    m_renderer.render();
}

}  // namespace chessfml::states
