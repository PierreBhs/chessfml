#pragma once

#include <SFML/Graphics.hpp>

namespace chessfml {

class renderer
{
public:
    explicit renderer(sf::RenderWindow& window) : m_window(window) {}
    virtual ~renderer() = default;

    virtual void render() = 0;

protected:
    sf::RenderWindow& m_window;
};

}  // namespace chessfml
