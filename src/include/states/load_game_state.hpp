#pragma once

#include <SFML/Graphics.hpp>

#include "game/board.hpp"
#include "game/game_state.hpp"
#include "states/state.hpp"

#include <array>
#include <filesystem>
#include <optional>
#include <string>

namespace chessfml {

enum class load_method { Direct, FromFile };
enum class player_choice { White, Black };

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

    enum class ui_component {
        FenInput,
        FileInput,
        RadioDirect,
        RadioFile,
        RadioWhite,
        RadioBlack,
        ButtonLoad,
        ButtonValidate,
        ButtonCancel,
        FenBrowse,
        Count
    };

    struct Component
    {
        sf::RectangleShape background;
        sf::Text           text{m_font};
        bool               active = false;
        bool               hovered = false;
    };

    // UI methods
    void               create_ui_components();
    void               update_component_states();
    [[nodiscard]] bool is_point_in_component(const sf::Vector2f& point, ui_component comp) const noexcept;
    void               handle_mouse_click(const sf::Vector2f& pos);
    void               handle_text_input(const sf::Event::TextEntered& text);
    void               handle_key_pressed(const sf::Event::KeyPressed& key);

    // FEN processing methods
    void               attempt_load_game();
    void               validate_fen();
    [[nodiscard]] bool load_fen_from_file(const std::string& filename);
    void               open_file_dialog();
    void               update_preview();

    std::array<Component, static_cast<size_t>(ui_component::Count)> m_components;
    std::string   m_fen_string = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1";  // Default FEN
    std::string   m_file_path;
    load_method   m_current_method = load_method::Direct;
    player_choice m_player_choice = player_choice::White;

    // Text cursor
    bool                   m_cursor_visible = true;
    float                  m_cursor_blink_timer = 0.0f;
    static constexpr float m_cursor_blink_interval = 0.5f;
    size_t                 m_cursor_position = 0;
    bool                   m_is_dragging = false;
    sf::Clock              m_cursor_clock;  // For cursor blinking

    bool   m_text_selected = false;
    size_t m_selection_start = 0;
    size_t m_selection_end = 0;

    // Game preview
    board_t     m_preview_board;
    game_state  m_preview_state;
    bool        m_fen_valid = true;
    std::string m_validation_message;

    // Filename dialog result
    std::optional<std::string> m_dialog_result;
};

}  // namespace chessfml
