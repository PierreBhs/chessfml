#include "ui/menu_renderer.hpp"

#include <print>

namespace chessfml {

menu_renderer::menu_renderer(sf::RenderWindow& window) : renderer(window)
{
    std::println("CREATING ITEMS");
    create_menu_items();
}

void menu_renderer::create_menu_items()
{
    sf::Vector2 window_size = m_window.getSize();

    float button_width = window_size.x * 0.4f;
    float button_height = window_size.y * 0.1f;
    float button_spacing = window_size.y * 0.05f;

    float start_y = (window_size.y - (3 * button_height + 2 * button_spacing)) / 2;

    for (int i = 0; i < 3; ++i) {
        float button_y = start_y + i * (button_height + button_spacing);

        // Setup rectangle shape
        m_menu_items[i].shape = sf::RectangleShape({button_width, button_height});
        m_menu_items[i].shape.setPosition({(window_size.x - button_width) / 2, button_y});
        m_menu_items[i].shape.setFillColor(sf::Color(100, 100, 100));
        m_menu_items[i].shape.setOutlineThickness(2);
        m_menu_items[i].shape.setOutlineColor(sf::Color::White);

        // Setup text
        m_menu_items[i].text = sf::Text(m_font, std::string(BUTTON_LABELS[i]), 24);
        m_menu_items[i].text.setFillColor(sf::Color::White);

        // Center text in button
        sf::FloatRect textBounds = m_menu_items[i].text.getLocalBounds();
        m_menu_items[i].text.setPosition({(window_size.x - textBounds.size.x) / 2,
                                          button_y + (button_height - textBounds.size.y) / 2 - textBounds.position.y});

        // Set first option as selected by default
        m_menu_items[i].selected = (i == 0);
    }

    // Highlight selected option
    m_menu_items[0].shape.setFillColor(sf::Color(150, 150, 150));
}

void menu_renderer::render()
{
    // Draw title
    sf::Text title(m_font, "Chess Game", 48);
    title.setFillColor(sf::Color::White);

    sf::FloatRect titleBounds = title.getLocalBounds();
    title.setPosition({(m_window.getSize().x - titleBounds.size.x) / 2, m_window.getSize().y * 0.1f});

    m_window.draw(title);

    // Draw menu items
    for (const auto& item : m_menu_items) {
        m_window.draw(item.shape);
        m_window.draw(item.text);
    }
}

void menu_renderer::set_hovered(std::size_t index, bool hovered)
{
    if (index < m_menu_items.size()) {
        m_menu_items[index].hovered = hovered;
        update_button_colors();
    }
}

bool menu_renderer::is_button_hovered(std::size_t index, sf::Vector2f mouse_pos) const
{
    if (index < m_menu_items.size()) {
        return m_menu_items[index].shape.getGlobalBounds().contains(sf::Vector2f(mouse_pos));
    }
    return false;
}

void menu_renderer::update_button_colors()
{
    for (auto i = 0u; i < m_menu_items.size(); ++i) {
        auto& item = m_menu_items[i];
        if (i == m_selected_option) {
            item.shape.setFillColor(sf::Color(150, 150, 150));  // Selected
        } else if (item.hovered) {
            item.shape.setFillColor(sf::Color(120, 120, 120));  // Hovered
        } else {
            item.shape.setFillColor(sf::Color(100, 100, 100));  // Normal
        }
    }
}
}  // namespace chessfml
