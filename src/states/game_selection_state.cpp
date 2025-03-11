#include "states/game_selection_state.hpp"

#include "common/config.hpp"
#include "states/play_state.hpp"
#include "states/state_manager.hpp"

#include <thread>

namespace chessfml {

game_selection_state::game_selection_state(sf::RenderWindow& window) : m_window(window) {}

void game_selection_state::init()
{
    create_menu_items();
}

void game_selection_state::create_menu_items()
{
    const float window_width = static_cast<float>(config::game::WIDTH);
    const float window_height = static_cast<float>(config::game::HEIGHT);

    float button_width = window_width * 0.5f;
    float button_height = window_height * 0.08f;
    float button_spacing = window_height * 0.04f;

    float start_y = window_height * 0.35f;

    for (size_t i = 0; i < static_cast<size_t>(game_option::Count); ++i) {
        float button_y = start_y + i * (button_height + button_spacing);

        m_menu_items[i].shape = sf::RectangleShape({button_width, button_height});
        m_menu_items[i].shape.setPosition({(window_width - button_width) / 2.0f, button_y});
        m_menu_items[i].shape.setFillColor(sf::Color(100, 100, 100));
        m_menu_items[i].shape.setOutlineThickness(2.0f);
        m_menu_items[i].shape.setOutlineColor(sf::Color::White);

        unsigned int font_size = static_cast<unsigned int>(window_height * 0.03f);
        m_menu_items[i].text = sf::Text(m_font, std::string(BUTTON_LABELS[i]), font_size);
        m_menu_items[i].text.setFillColor(sf::Color::White);

        sf::FloatRect textBounds = m_menu_items[i].text.getLocalBounds();
        m_menu_items[i].text.setPosition(
            {(window_width - textBounds.size.x) / 2.0f,
             button_y + (button_height - textBounds.size.y) / 2.0f - textBounds.position.y});

        m_menu_items[i].selected = (i == 0);
    }

    update_button_colors();
}

void game_selection_state::handle_event(const sf::Event& event)
{
    if (const auto* key_pressed = event.getIf<sf::Event::KeyPressed>()) {
        if (key_pressed->scancode == sf::Keyboard::Scancode::Up) {
            m_selected_option = static_cast<game_option>(
                (static_cast<int>(m_selected_option) - 1 + static_cast<int>(game_option::Count)) %
                static_cast<int>(game_option::Count));
            update_button_colors();
        } else if (key_pressed->scancode == sf::Keyboard::Scancode::Down) {
            m_selected_option = static_cast<game_option>((static_cast<int>(m_selected_option) + 1) %
                                                         static_cast<int>(game_option::Count));
            update_button_colors();
        } else if (key_pressed->scancode == sf::Keyboard::Scancode::Enter) {
            activate_selected_option();
        } else if (key_pressed->scancode == sf::Keyboard::Scancode::Escape) {
            m_manager->pop_state();
        }
    } else if (event.getIf<sf::Event::MouseMoved>()) {
        update_component_states();
    } else if (const auto* mouse_pressed = event.getIf<sf::Event::MouseButtonPressed>()) {
        if (mouse_pressed->button == sf::Mouse::Button::Left) {
            handle_mouse_click(
                {static_cast<float>(mouse_pressed->position.x), static_cast<float>(mouse_pressed->position.y)});
        }
    }
}

void game_selection_state::update([[maybe_unused]] float dt)
{
    std::this_thread::sleep_for(std::chrono::milliseconds{30});
}

void game_selection_state::render()
{
    m_window.clear(sf::Color(40, 40, 40));

    sf::Text title(m_font, "Select Game Mode", 48);
    title.setFillColor(sf::Color::White);
    sf::FloatRect titleBounds = title.getLocalBounds();
    title.setPosition({(static_cast<float>(config::game::WIDTH) - titleBounds.size.x) / 2.0f,
                       static_cast<float>(config::game::HEIGHT) * 0.15f});
    m_window.draw(title);

    // Draw subtitle instruction
    sf::Text subtitle(m_font, "Choose how you want to play", 24);
    subtitle.setFillColor(sf::Color(200, 200, 200));
    sf::FloatRect subtitleBounds = subtitle.getLocalBounds();
    subtitle.setPosition({(static_cast<float>(config::game::WIDTH) - subtitleBounds.size.x) / 2.0f,
                          static_cast<float>(config::game::HEIGHT) * 0.25f});
    m_window.draw(subtitle);

    for (const auto& item : m_menu_items) {
        m_window.draw(item.shape);
        m_window.draw(item.text);
    }
}

void game_selection_state::update_component_states()
{
    sf::Vector2i mouse_pos = sf::Mouse::getPosition(m_window);
    sf::Vector2f mouse_pos_f = static_cast<sf::Vector2f>(mouse_pos);

    for (size_t i = 0; i < m_menu_items.size(); ++i) {
        m_menu_items[i].hovered = is_point_in_component(mouse_pos_f, i);
    }

    update_button_colors();
}

bool game_selection_state::is_point_in_component(const sf::Vector2f& point, size_t index) const
{
    if (index < m_menu_items.size()) {
        return m_menu_items[index].shape.getGlobalBounds().contains(point);
    }
    return false;
}

void game_selection_state::handle_mouse_click(const sf::Vector2f& pos)
{
    for (size_t i = 0; i < m_menu_items.size(); ++i) {
        if (is_point_in_component(pos, i)) {
            m_selected_option = static_cast<game_option>(i);
            update_button_colors();
            activate_selected_option();
            break;
        }
    }
}

void game_selection_state::activate_selected_option()
{
    switch (m_selected_option) {
        case game_option::HumanVsHuman:
            m_manager->replace_state<play_state>(m_window, player_t::Human, player_t::Human);
            break;
        case game_option::HumanVsAI_White:
            m_manager->replace_state<play_state>(m_window, player_t::Human, player_t::AI);
            break;
        case game_option::HumanVsAI_Black:
            m_manager->replace_state<play_state>(m_window, player_t::AI, player_t::Human);
            break;
        case game_option::Back:
            m_manager->pop_state();
            break;
        default:
            break;
    }
}

void game_selection_state::update_button_colors()
{
    for (size_t i = 0; i < m_menu_items.size(); ++i) {
        auto& item = m_menu_items[i];
        if (static_cast<game_option>(i) == m_selected_option) {
            item.shape.setFillColor(sf::Color(150, 150, 150));  // Selected
        } else if (item.hovered) {
            item.shape.setFillColor(sf::Color(120, 120, 120));  // Hovered
        } else {
            item.shape.setFillColor(sf::Color(100, 100, 100));  // Normal
        }
    }
}

}  // namespace chessfml
