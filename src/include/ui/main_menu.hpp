#pragma once

#include <SFML/Graphics.hpp>
#include <array>
#include <string_view>

namespace chessfml {

enum class AppState { MainMenu, Game, LoadGame, GameOver, Exit };

class Button
{
public:
    Button(sf::Vector2f position, sf::Vector2f size, std::string_view text, const sf::Font& font);

    void render(sf::RenderTarget& target);
    bool contains(const sf::Vector2f& point) const;
    void set_hovered(bool hovered);

private:
    sf::RectangleShape m_shape;
    sf::Text           m_text;
    bool               m_hovered{false};
};

class MainMenu
{
public:
    MainMenu(AppState& current_state, sf::RenderWindow& window);

    void process_events(const sf::Event& event);
    void update(double dt);
    void render();

    const sf::Font& get_font() const { return m_font; }

private:
    void create_buttons();

    AppState&           m_app_state;
    sf::RenderWindow&   m_window;
    sf::Font            m_font;
    std::vector<Button> m_buttons;

    static constexpr std::array<std::string_view, 3> BUTTON_LABELS = {"Play", "Load Game", "Exit"};
};

}  // namespace chessfml
