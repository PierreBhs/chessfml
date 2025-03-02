#include "ui/menu_renderer.hpp"

#include "config/config.hpp"

namespace chessfml {

menu_renderer::menu_renderer(sf::RenderWindow& window) : renderer(window)
{
    create_menu_items();
}

void menu_renderer::create_menu_items()
{
    const float window_width = static_cast<float>(config::game::WIDTH);
    const float window_height = static_cast<float>(config::game::HEIGHT);

    float button_width = window_width * 0.4f;
    float button_height = window_height * 0.08f;
    float button_spacing = window_height * 0.04f;

    float start_y = window_height * 0.35f;
    for (int i = 0; i < 3; ++i) {
        float button_y = start_y + i * (button_height + button_spacing);

        m_menu_items[i].shape = sf::RectangleShape({button_width, button_height});
        m_menu_items[i].shape.setPosition({(window_width - button_width) / 2.0f,  // Center horizontally
                                           button_y});
        m_menu_items[i].shape.setFillColor(sf::Color(100, 100, 100));
        m_menu_items[i].shape.setOutlineThickness(2.0f);
        m_menu_items[i].shape.setOutlineColor(sf::Color::White);

        unsigned int font_size = static_cast<unsigned int>(window_height * 0.03f);  // 3% of height
        m_menu_items[i].text = sf::Text(m_font, std::string(BUTTON_LABELS[i]), font_size);
        m_menu_items[i].text.setFillColor(sf::Color::White);

        sf::FloatRect textBounds = m_menu_items[i].text.getLocalBounds();
        m_menu_items[i].text.setPosition({
            (window_width - textBounds.size.x) / 2.0f,                                     // Center horizontally
            button_y + (button_height - textBounds.size.y) / 2.0f - textBounds.position.y  // Center vertically
        });

        m_menu_items[i].selected = (i == 0);
    }

    update_button_colors();
}

void menu_renderer::render()
{
    const float window_width = static_cast<float>(config::game::WIDTH);
    const float window_height = static_cast<float>(config::game::HEIGHT);

    unsigned int title_size = static_cast<unsigned int>(window_height * 0.05f);  // 5% of height
    sf::Text     title(m_font, "ChesSfml", title_size);
    title.setFillColor(sf::Color::White);

    sf::FloatRect titleBounds = title.getLocalBounds();
    title.setPosition({
        (window_width - titleBounds.size.x) / 2.0f,  // Center horizontally
        window_height * 0.15f                        // 15% down from top
    });

    m_window.draw(title);

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
