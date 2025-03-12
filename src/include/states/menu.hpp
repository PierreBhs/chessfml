#pragma once

#include "states/state.hpp"
#include "ui/menu_renderer.hpp"

namespace chessfml::states {

class menu : public state
{
public:
    menu(sf::RenderWindow& window);
    ~menu() override = default;

    void init() override;
    void handle_event(const sf::Event& event) override;
    void update(float dt) override;
    void render() override;

private:
    enum class menu_option { Play, LoadGame, Exit, Count };

    void activate_selected_option();

    sf::RenderWindow& m_window;
    menu_renderer     m_renderer;
    int               m_selected_option{0};
};

}  // namespace chessfml::states
