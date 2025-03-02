#pragma once

#include <SFML/Graphics.hpp>
#include <chrono>
#include "states/state.hpp"

namespace chessfml {

class load_game_state : public state
{
public:
    load_game_state(sf::RenderWindow& window);
    ~load_game_state() override = default;

    void init() override;
    void handle_event(const sf::Event& event) override;
    void update(float dt) override;
    void render() override;

private:
    sf::RenderWindow&            m_window;
    inline static const sf::Font m_font{"data/fonts/Montserrat-Regular.ttf"};
    sf::Text                     m_message{m_font, "Load Game feature coming soon...", 32};

    std::chrono::time_point<std::chrono::steady_clock> m_start_time;
    float                                              m_display_time{2.0f};  // Time in seconds to display the message
};

}  // namespace chessfml
