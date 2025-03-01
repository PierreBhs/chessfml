#include "ui/main_menu.hpp"

#include <print>

namespace chessfml {

Button::Button(sf::Vector2f position, sf::Vector2f size, std::string_view text, const sf::Font& font)
    : m_shape(size), m_text(font, std::string{text}, 55)
{
    m_shape.setPosition(position);
    m_shape.setFillColor(sf::Color(100, 100, 100));
    m_shape.setOutlineThickness(2);
    m_shape.setOutlineColor(sf::Color::White);

    m_text.setFont(font);
    m_text.setString(std::string(text));
    m_text.setCharacterSize(24);
    m_text.setFillColor(sf::Color::White);

    sf::FloatRect textBounds = m_text.getLocalBounds();
    m_text.setPosition({position.x + (size.x - textBounds.size.x) / 2,
                        position.y + (size.y - textBounds.size.y) / 2 - textBounds.position.y});
}

void Button::render(sf::RenderTarget& target)
{
    target.draw(m_shape);
    target.draw(m_text);
}

bool Button::contains(const sf::Vector2f& point) const
{
    return m_shape.getGlobalBounds().contains(point);
}

void Button::set_hovered(bool hovered)
{
    m_hovered = hovered;
    m_shape.setFillColor(hovered ? sf::Color(150, 150, 150) : sf::Color(100, 100, 100));
}

MainMenu::MainMenu(AppState& current_state, sf::RenderWindow& window) : m_app_state(current_state), m_window(window)
{
    if (!m_font.openFromFile("data/fonts/Montserrat-Regular.ttf")) {
        std::println("couldnt open font");
    }

    create_buttons();
}

void MainMenu::create_buttons()
{
    sf::Vector2 windowSize = m_window.getSize();

    float button_width = windowSize.x * 0.4f;
    float button_height = windowSize.y * 0.1f;
    float button_spacing = windowSize.y * 0.05f;

    float start_y = (windowSize.y - (3 * button_height + 2 * button_spacing)) / 2;

    for (int i = 0; i < 3; ++i) {
        float button_y = start_y + i * (button_height + button_spacing);

        m_buttons.emplace_back(Button(sf::Vector2f((windowSize.x - button_width) / 2, button_y),
                                      sf::Vector2f(button_width, button_height),
                                      BUTTON_LABELS[i],
                                      m_font));
    }
}

void MainMenu::process_events(const sf::Event& event)
{
    if (const auto* key_pressed = event.getIf<sf::Event::KeyPressed>()) {
        if (key_pressed->scancode == sf::Keyboard::Scancode::Escape) {
            m_app_state = AppState::Exit;
        }
    } else if (const auto* mouse_moved = event.getIf<sf::Event::MouseMoved>()) {
        sf::Vector2 mouse_pos = mouse_moved->position;

        for (auto& button : m_buttons) {
            button.set_hovered(button.contains(sf::Vector2f{mouse_pos}));
        }
    } else if (const auto* mouse_pressed = event.getIf<sf::Event::MouseButtonPressed>()) {
        if (mouse_pressed->button == sf::Mouse::Button::Left) {
            sf::Vector2 mouse_pos = mouse_pressed->position;

            for (auto i = 0u; i < m_buttons.size(); ++i) {
                if (m_buttons[i].contains(sf::Vector2f{mouse_pos})) {
                    switch (i) {
                        case 0:  // Play
                            m_app_state = AppState::Game;
                            break;
                        case 1:  // Load Game
                            m_app_state = AppState::LoadGame;
                            break;
                        case 2:  // Exit
                            m_app_state = AppState::Exit;
                            break;
                    }
                    break;
                }
            }
        }
    }
}

void MainMenu::update([[maybe_unused]] double dt) {}

void MainMenu::render()
{
    m_window.clear(sf::Color(50, 50, 50));

    sf::Text title(m_font, "ChesSfml", 48);
    title.setFillColor(sf::Color::White);
    sf::FloatRect titleBounds = title.getLocalBounds();
    title.setPosition({(m_window.getSize().x - titleBounds.size.x) / 2, m_window.getSize().y * 0.1f});

    m_window.draw(title);

    for (auto& button : m_buttons) {
        button.render(m_window);
    }

    m_window.display();
}

}  // namespace chessfml
