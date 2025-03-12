#pragma once

#include "common/font.hpp"
#include "states/state.hpp"

#include <SFML/Graphics.hpp>

namespace chessfml::states {

// enum class player_t { Human, AI };

class game_selection : public state
{
public:
    game_selection(sf::RenderWindow& window);
    ~game_selection() override = default;

    void init() override;
    void handle_event(const sf::Event& event) override;
    void update(float dt) override;
    void render() override;

private:
    enum class game_option {
        HumanVsHuman,
        HumanVsAI_White,  // Human plays as White
        HumanVsAI_Black,  // Human plays as Black
        Back,
        Count
    };

    struct menu_item
    {
        sf::Text           text{m_font};
        sf::RectangleShape shape;
        bool               selected{false};
        bool               hovered{false};
    };

    void create_menu_items();
    void update_component_states();
    bool is_point_in_component(const sf::Vector2f& point, size_t index) const;
    void handle_mouse_click(const sf::Vector2f& pos);
    void activate_selected_option();
    void update_button_colors();

    sf::RenderWindow&                                              m_window;
    inline static const sf::Font                                   m_font{get_font()};
    std::array<menu_item, static_cast<size_t>(game_option::Count)> m_menu_items;
    game_option                                                    m_selected_option{game_option::HumanVsHuman};

    static constexpr std::array<std::string_view, static_cast<size_t>(game_option::Count)> BUTTON_LABELS = {
        "Human vs Human",
        "Human vs AI (as White)",
        "Human vs AI (as Black)",
        "Back to Main Menu"};
};

}  // namespace chessfml::states
