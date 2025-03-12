#pragma once

#include <SFML/Graphics/Font.hpp>

namespace chessfml {

class font_singleton
{

public:
    font_singleton(const font_singleton&) = delete;
    font_singleton& operator=(const font_singleton&) = delete;

    const sf::Font& get_font() const { return m_font; }

    static font_singleton& instance()
    {
        static font_singleton instance;
        return instance;
    }

private:
    font_singleton() = default;
    ~font_singleton() = default;

    sf::Font m_font{"data/fonts/Montserrat-Regular.ttf"};
};

inline const sf::Font& get_font()
{
    return font_singleton::instance().get_font();
}

}  // namespace chessfml
