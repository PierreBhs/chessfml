#pragma once

#include <array>
#include "ui/renderer.hpp"

namespace chessfml {

class menu_renderer : public renderer
{
public:
    explicit menu_renderer(sf::RenderWindow& window);
    ~menu_renderer() override = default;

    void render() override;

    void set_selected_option(int option)
    {
        m_selected_option = option;
        update_button_colors();
    }
    void set_hovered(std::size_t index, bool hovered);
    bool is_button_hovered(std::size_t index, sf::Vector2f mouse_pos) const;

    const sf::Font& get_font() const { return m_font; }

private:
    struct menu_item
    {
        sf::Text           text{m_font};
        sf::RectangleShape shape{};
        bool               selected{false};
        bool               hovered{false};
    };

    void create_menu_items();
    void update_button_colors();

    inline static const sf::Font m_font{"data/fonts/Montserrat-Regular.ttf"};
    std::array<menu_item, 3>     m_menu_items;
    std::size_t                  m_selected_option{0u};

    static constexpr std::array<std::string_view, 3> BUTTON_LABELS = {"Play", "Load Game", "Exit"};
};

}  // namespace chessfml
