#include "states/load_game_state.hpp"

#include "common/config.hpp"
#include "common/fen.hpp"
#include "states/play_state.hpp"
#include "states/state_manager.hpp"

#include <SFML/Window/Keyboard.hpp>
#include <filesystem>
#include <format>
#include <fstream>
#include <print>
#include <thread>

namespace chessfml {

load_game_state::load_game_state(sf::RenderWindow& window) : m_window(window) {}

void load_game_state::init()
{
    create_ui_components();
    update_preview();
}

void load_game_state::create_ui_components()
{
    const float window_width = static_cast<float>(config::game::WIDTH);
    const float window_height = static_cast<float>(config::game::HEIGHT);

    // FEN Input field
    auto& fen_input = m_components[static_cast<size_t>(ui_component::FenInput)];
    fen_input.background.setSize({window_width * 0.7f, 40.0f});
    fen_input.background.setPosition({window_width * 0.15f, window_height * 0.2f});
    fen_input.background.setFillColor(sf::Color(240, 240, 240));
    fen_input.background.setOutlineThickness(2.0f);
    fen_input.background.setOutlineColor(sf::Color(100, 100, 100));
    fen_input.text = sf::Text(m_font, m_fen_string, 18);
    fen_input.text.setFillColor(sf::Color::Black);
    fen_input.text.setPosition(fen_input.background.getPosition() + sf::Vector2f(10.0f, 10.0f));
    fen_input.active = true;

    // File Input field
    auto& file_input = m_components[static_cast<size_t>(ui_component::FileInput)];
    file_input.background.setSize({window_width * 0.6f, 40.0f});
    file_input.background.setPosition({window_width * 0.15f, window_height * 0.3f});
    file_input.background.setFillColor(sf::Color(240, 240, 240));
    file_input.background.setOutlineThickness(2.0f);
    file_input.background.setOutlineColor(sf::Color(100, 100, 100));
    file_input.text = sf::Text(m_font, "Click Browse to select a file...", 18);
    file_input.text.setFillColor(sf::Color(100, 100, 100));
    file_input.text.setPosition(file_input.background.getPosition() + sf::Vector2f(10.0f, 10.0f));

    // Browse button
    auto& browse_button = m_components[static_cast<size_t>(ui_component::FenBrowse)];
    browse_button.background.setSize({window_width * 0.09f, 40.0f});
    browse_button.background.setPosition({window_width * 0.76f, window_height * 0.3f});
    browse_button.background.setFillColor(sf::Color(180, 180, 180));
    browse_button.background.setOutlineThickness(2.0f);
    browse_button.background.setOutlineColor(sf::Color(100, 100, 100));
    browse_button.text = sf::Text(m_font, "Browse", 18);
    browse_button.text.setFillColor(sf::Color::Black);
    sf::FloatRect textBounds = browse_button.text.getLocalBounds();
    browse_button.text.setPosition(browse_button.background.getPosition() +
                                   sf::Vector2f((browse_button.background.getSize().x - textBounds.size.x) / 2,
                                                (browse_button.background.getSize().y - textBounds.size.y) / 2 - 5.0f));

    // Radio buttons for input method
    float radio_y = window_height * 0.4f;
    float radio_spacing = 150.0f;

    // Direct input radio
    auto& radio_direct = m_components[static_cast<size_t>(ui_component::RadioDirect)];
    radio_direct.background.setSize({20.0f, 20.0f});
    radio_direct.background.setPosition({window_width * 0.3f - 60.0f, radio_y});
    radio_direct.background.setFillColor(sf::Color::White);
    radio_direct.background.setOutlineThickness(2.0f);
    radio_direct.background.setOutlineColor(sf::Color::Black);
    radio_direct.text = sf::Text(m_font, "Direct Input", 18);
    radio_direct.text.setFillColor(sf::Color::White);
    radio_direct.text.setPosition(radio_direct.background.getPosition() + sf::Vector2f(30.0f, -2.0f));
    radio_direct.active = true;

    // File input radio
    auto& radio_file = m_components[static_cast<size_t>(ui_component::RadioFile)];
    radio_file.background.setSize({20.0f, 20.0f});
    radio_file.background.setPosition({window_width * 0.3f - 60.0f + radio_spacing, radio_y});
    radio_file.background.setFillColor(sf::Color::White);
    radio_file.background.setOutlineThickness(2.0f);
    radio_file.background.setOutlineColor(sf::Color::Black);
    radio_file.text = sf::Text(m_font, "From File", 18);
    radio_file.text.setFillColor(sf::Color::White);
    radio_file.text.setPosition(radio_file.background.getPosition() + sf::Vector2f(30.0f, -2.0f));

    // Radio buttons for player choice
    float player_radio_y = window_height * 0.5f;

    // White player radio
    auto& radio_white = m_components[static_cast<size_t>(ui_component::RadioWhite)];
    radio_white.background.setSize({20.0f, 20.0f});
    radio_white.background.setPosition({window_width * 0.3f - 60.0f, player_radio_y});
    radio_white.background.setFillColor(sf::Color::White);
    radio_white.background.setOutlineThickness(2.0f);
    radio_white.background.setOutlineColor(sf::Color::Black);
    radio_white.text = sf::Text(m_font, "White Starts", 18);
    radio_white.text.setFillColor(sf::Color::White);
    radio_white.text.setPosition(radio_white.background.getPosition() + sf::Vector2f(30.0f, -2.0f));
    radio_white.active = true;

    // Black player radio
    auto& radio_black = m_components[static_cast<size_t>(ui_component::RadioBlack)];
    radio_black.background.setSize({20.0f, 20.0f});
    radio_black.background.setPosition({window_width * 0.3f - 60.0f + radio_spacing, player_radio_y});
    radio_black.background.setFillColor(sf::Color::White);
    radio_black.background.setOutlineThickness(2.0f);
    radio_black.background.setOutlineColor(sf::Color::Black);
    radio_black.text = sf::Text(m_font, "Black Starts", 18);
    radio_black.text.setFillColor(sf::Color::White);
    radio_black.text.setPosition(radio_black.background.getPosition() + sf::Vector2f(30.0f, -2.0f));

    // Buttons
    float button_width = 150.0f;
    float button_height = 40.0f;
    float button_y = window_height * 0.7f;
    float button_spacing = window_width * 0.15f;

    // Load button
    auto& load_button = m_components[static_cast<size_t>(ui_component::ButtonLoad)];
    load_button.background = sf::RectangleShape({button_width, button_height});
    load_button.background.setPosition({window_width * 0.5f - button_width - button_spacing, button_y});
    load_button.background.setFillColor(sf::Color(100, 180, 100));
    load_button.background.setOutlineThickness(2.0f);
    load_button.background.setOutlineColor(sf::Color(60, 100, 60));
    load_button.text = sf::Text(m_font, "Load Game", 18);
    load_button.text.setFillColor(sf::Color::White);
    textBounds = load_button.text.getLocalBounds();
    load_button.text.setPosition(load_button.background.getPosition() +
                                 sf::Vector2f((load_button.background.getSize().x - textBounds.size.x) / 2,
                                              (load_button.background.getSize().y - textBounds.size.y) / 2 - 5.0f));

    // Validate button
    auto& validate_button = m_components[static_cast<size_t>(ui_component::ButtonValidate)];
    validate_button.background = sf::RectangleShape({button_width, button_height});
    validate_button.background.setPosition({window_width * 0.5f, button_y});
    validate_button.background.setFillColor(sf::Color(100, 100, 180));
    validate_button.background.setOutlineThickness(2.0f);
    validate_button.background.setOutlineColor(sf::Color(60, 60, 100));
    validate_button.text = sf::Text(m_font, "Validate FEN", 18);
    validate_button.text.setFillColor(sf::Color::White);
    textBounds = validate_button.text.getLocalBounds();
    validate_button.text.setPosition(
        validate_button.background.getPosition() +
        sf::Vector2f((validate_button.background.getSize().x - textBounds.size.x) / 2,
                     (validate_button.background.getSize().y - textBounds.size.y) / 2 - 5.0f));

    // Cancel button
    auto& cancel_button = m_components[static_cast<size_t>(ui_component::ButtonCancel)];
    cancel_button.background = sf::RectangleShape({button_width, button_height});
    cancel_button.background.setPosition({window_width * 0.5f + button_width + button_spacing, button_y});
    cancel_button.background.setFillColor(sf::Color(180, 100, 100));
    cancel_button.background.setOutlineThickness(2.0f);
    cancel_button.background.setOutlineColor(sf::Color(100, 60, 60));
    cancel_button.text = sf::Text(m_font, "Cancel", 18);
    cancel_button.text.setFillColor(sf::Color::White);
    textBounds = cancel_button.text.getLocalBounds();
    cancel_button.text.setPosition(cancel_button.background.getPosition() +
                                   sf::Vector2f((cancel_button.background.getSize().x - textBounds.size.x) / 2,
                                                (cancel_button.background.getSize().y - textBounds.size.y) / 2 - 5.0f));
}

void load_game_state::handle_event(const sf::Event& event)
{
    if (const auto* key_event = event.getIf<sf::Event::KeyPressed>()) {
        if (key_event->code == sf::Keyboard::Key::V && sf::Keyboard::isKeyPressed(sf::Keyboard::Key::LControl)) {
            if (m_components[static_cast<size_t>(ui_component::FenInput)].active) {
                std::string clipboard = sf::Clipboard::getString().toAnsiString();
                if (!clipboard.empty()) {
                    m_fen_string.insert(m_cursor_position, clipboard);
                    m_cursor_position += clipboard.length();
                    m_components[static_cast<size_t>(ui_component::FenInput)].text.setString(m_fen_string);
                }
                return;
            }
        }
        // Select all text shortcut (Ctrl+A)
        else if (key_event->code == sf::Keyboard::Key::A && sf::Keyboard::isKeyPressed(sf::Keyboard::Key::LControl)) {
            if (m_components[static_cast<size_t>(ui_component::FenInput)].active) {
                // Select all text
                m_text_selected = true;
                m_selection_start = 0;
                m_selection_end = m_fen_string.length();
                m_cursor_position = m_fen_string.length();
                return;
            }
        }
    }

    if (event.is<sf::Event::MouseMoved>()) {
        update_component_states();
    } else if (const auto* mouse_pressed = event.getIf<sf::Event::MouseButtonPressed>()) {
        if (mouse_pressed->button == sf::Mouse::Button::Left) {
            handle_mouse_click(
                {static_cast<float>(mouse_pressed->position.x), static_cast<float>(mouse_pressed->position.y)});
        }
    } else if (const auto* text_entered = event.getIf<sf::Event::TextEntered>()) {
        if (m_current_method == load_method::Direct &&
            m_components[static_cast<size_t>(ui_component::FenInput)].active && text_entered->unicode >= 32 &&
            text_entered->unicode < 128) {

            m_fen_string.insert(m_cursor_position, 1, static_cast<char>(text_entered->unicode));
            m_cursor_position++;
            m_components[static_cast<size_t>(ui_component::FenInput)].text.setString(m_fen_string);
        }
    } else if (const auto* key_pressed = event.getIf<sf::Event::KeyPressed>()) {
        handle_key_pressed(*key_pressed);
    }
}

void load_game_state::update([[maybe_unused]] float dt)
{
    if (m_cursor_clock.getElapsedTime().asSeconds() >= m_cursor_blink_interval) {
        m_cursor_visible = !m_cursor_visible;
        m_cursor_clock.restart();
    }

    // Check for file dialog result
    if (m_dialog_result) {
        m_file_path = *m_dialog_result;
        m_components[static_cast<size_t>(ui_component::FileInput)].text.setString(m_file_path);
        m_components[static_cast<size_t>(ui_component::FileInput)].text.setFillColor(sf::Color::Black);
        m_dialog_result.reset();

        // Load FEN from file
        if (!m_file_path.empty() && load_fen_from_file(m_file_path)) {
            // Update FEN input field with loaded content
            m_components[static_cast<size_t>(ui_component::FenInput)].text.setString(m_fen_string);
            validate_fen();
            update_preview();
        }
    }

    std::this_thread::sleep_for(std::chrono::milliseconds{30});
}

void load_game_state::render()
{
    m_window.clear(sf::Color(40, 40, 40));

    sf::Text title(m_font, "Load Chess Game", 32);
    title.setFillColor(sf::Color::White);
    sf::FloatRect titleBounds = title.getLocalBounds();
    title.setPosition({(static_cast<float>(config::game::WIDTH) - titleBounds.size.x) / 2.0f,
                       static_cast<float>(config::game::HEIGHT) * 0.1f});
    m_window.draw(title);

    sf::Text fen_label(m_font, "FEN String:", 20);
    fen_label.setFillColor(sf::Color::White);
    fen_label.setPosition(
        {m_components[static_cast<size_t>(ui_component::FenInput)].background.getPosition().x,
         m_components[static_cast<size_t>(ui_component::FenInput)].background.getPosition().y - 30.0f});
    m_window.draw(fen_label);

    sf::Text file_label(m_font, "Load from File:", 20);
    file_label.setFillColor(sf::Color::White);
    file_label.setPosition(
        {m_components[static_cast<size_t>(ui_component::FileInput)].background.getPosition().x,
         m_components[static_cast<size_t>(ui_component::FileInput)].background.getPosition().y - 30.0f});
    m_window.draw(file_label);

    sf::Text method_label(m_font, "Input Method:", 20);
    method_label.setFillColor(sf::Color::White);
    method_label.setPosition(
        {m_components[static_cast<size_t>(ui_component::RadioDirect)].background.getPosition().x,
         m_components[static_cast<size_t>(ui_component::RadioDirect)].background.getPosition().y - 30.0f});
    m_window.draw(method_label);

    sf::Text player_label(m_font, "Starting Player:", 20);
    player_label.setFillColor(sf::Color::White);
    player_label.setPosition(
        {m_components[static_cast<size_t>(ui_component::RadioWhite)].background.getPosition().x,
         m_components[static_cast<size_t>(ui_component::RadioWhite)].background.getPosition().y - 30.0f});
    m_window.draw(player_label);

    // Draw all UI components
    for (const auto& component : m_components) {
        m_window.draw(component.background);

        // For radio buttons, draw selection circle if active
        for (auto radio :
             {ui_component::RadioDirect, ui_component::RadioFile, ui_component::RadioWhite, ui_component::RadioBlack}) {
            if (&component == &m_components[static_cast<size_t>(radio)]) {
                if (component.active) {
                    sf::CircleShape inner_circle(5.0f);
                    inner_circle.setFillColor(sf::Color::Black);
                    inner_circle.setPosition(component.background.getPosition() + sf::Vector2f(5.0f, 5.0f));
                    m_window.draw(inner_circle);
                }
                break;
            }
        }

        m_window.draw(component.text);
    }

    // Draw validation message
    sf::Text validation_text(m_font, m_validation_message, 18);
    validation_text.setFillColor(m_fen_valid ? sf::Color::Green : sf::Color::Red);
    validation_text.setPosition(
        {static_cast<float>(config::game::WIDTH) * 0.15f, static_cast<float>(config::game::HEIGHT) * 0.6f});
    m_window.draw(validation_text);

    if (m_text_selected && m_components[static_cast<size_t>(ui_component::FenInput)].active) {
        auto&     fen_input = m_components[static_cast<size_t>(ui_component::FenInput)];
        sf::Text& text = fen_input.text;
        sf::Text  start_text = text;
        start_text.setString(m_fen_string.substr(0, m_selection_start));

        sf::Text selected_text = text;
        selected_text.setString(m_fen_string.substr(m_selection_start, m_selection_end - m_selection_start));

        float start_x = text.getPosition().x + start_text.getGlobalBounds().size.x;
        float selection_width = selected_text.getGlobalBounds().size.x;

        sf::RectangleShape selection;
        selection.setPosition({start_x, text.getPosition().y});
        selection.setSize({selection_width, static_cast<float>(text.getCharacterSize())});
        selection.setFillColor(sf::Color(0, 120, 215, 128));  // Semi-transparent blue

        m_window.draw(selection);
    }

    if (m_components[static_cast<size_t>(ui_component::FenInput)].active && m_cursor_visible) {
        auto& fen_input = m_components[static_cast<size_t>(ui_component::FenInput)];

        sf::Text& text = fen_input.text;
        sf::Text  cursor_pos_text = text;
        cursor_pos_text.setString(m_fen_string.substr(0, m_cursor_position));

        float cursor_x = text.getPosition().x + cursor_pos_text.getGlobalBounds().size.x;

        sf::RectangleShape cursor;
        cursor.setSize({2.0f, static_cast<float>(text.getCharacterSize() + 4)});
        cursor.setFillColor(sf::Color::Black);
        cursor.setPosition({cursor_x, text.getPosition().y - 2.0f});
        m_window.draw(cursor);
    }
}

void load_game_state::update_component_states()
{
    sf::Vector2i mouse_pos = sf::Mouse::getPosition(m_window);
    sf::Vector2f mouse_pos_f = static_cast<sf::Vector2f>(mouse_pos);

    // Update button hover states
    for (size_t i = 0; i < m_components.size(); ++i) {
        auto&        component = m_components[i];
        ui_component comp_type = static_cast<ui_component>(i);

        if (comp_type == ui_component::FenInput) {
            continue;
        }

        component.hovered = is_point_in_component(mouse_pos_f, comp_type);

        if (comp_type == ui_component::ButtonLoad || comp_type == ui_component::ButtonValidate ||
            comp_type == ui_component::ButtonCancel || comp_type == ui_component::FenBrowse) {

            sf::Color base_color;
            sf::Color hover_color;

            if (comp_type == ui_component::ButtonLoad) {
                base_color = sf::Color(100, 180, 100);
                hover_color = sf::Color(120, 200, 120);
            } else if (comp_type == ui_component::ButtonValidate) {
                base_color = sf::Color(100, 100, 180);
                hover_color = sf::Color(120, 120, 200);
            } else if (comp_type == ui_component::ButtonCancel) {
                base_color = sf::Color(180, 100, 100);
                hover_color = sf::Color(200, 120, 120);
            } else {  // FenBrowse
                base_color = sf::Color(180, 180, 180);
                hover_color = sf::Color(200, 200, 200);
            }

            component.background.setFillColor(component.hovered ? hover_color : base_color);
        }
    }
}

bool load_game_state::is_point_in_component(const sf::Vector2f& point, ui_component comp) const noexcept
{
    const auto& component = m_components[static_cast<size_t>(comp)];
    return component.background.getGlobalBounds().contains(point);
}

void load_game_state::handle_mouse_click(const sf::Vector2f& pos)
{
    auto& fen_input = m_components[static_cast<size_t>(ui_component::FenInput)];

    if (fen_input.background.getGlobalBounds().contains(pos)) {
        fen_input.active = true;

        sf::Text& text = fen_input.text;
        float     text_x = pos.x - text.getPosition().x;

        float text_width = text.getGlobalBounds().size.x;
        float avg_char_width = text_width / static_cast<float>(m_fen_string.length() > 0 ? m_fen_string.length() : 1);

        m_cursor_position = static_cast<size_t>(text_x / avg_char_width);

        if (m_cursor_position > m_fen_string.length()) {
            m_cursor_position = m_fen_string.length();
        }

        m_cursor_clock.restart();
        m_cursor_visible = true;

        return;
    }

    for (size_t i = 0; i < m_components.size(); ++i) {
        ui_component comp_type = static_cast<ui_component>(i);

        if (is_point_in_component(pos, comp_type)) {
            switch (comp_type) {
                case ui_component::FenInput:
                    m_components[static_cast<size_t>(ui_component::FenInput)].active = true;
                    break;

                case ui_component::RadioDirect:
                    m_current_method = load_method::Direct;
                    m_components[static_cast<size_t>(ui_component::RadioDirect)].active = true;
                    m_components[static_cast<size_t>(ui_component::RadioFile)].active = false;
                    break;

                case ui_component::RadioFile:
                    m_current_method = load_method::FromFile;
                    m_components[static_cast<size_t>(ui_component::RadioDirect)].active = false;
                    m_components[static_cast<size_t>(ui_component::RadioFile)].active = true;
                    break;

                case ui_component::RadioWhite:
                    m_player_choice = player_choice::White;
                    m_components[static_cast<size_t>(ui_component::RadioWhite)].active = true;
                    m_components[static_cast<size_t>(ui_component::RadioBlack)].active = false;
                    break;

                case ui_component::RadioBlack:
                    m_player_choice = player_choice::Black;
                    m_components[static_cast<size_t>(ui_component::RadioWhite)].active = false;
                    m_components[static_cast<size_t>(ui_component::RadioBlack)].active = true;
                    break;

                case ui_component::ButtonLoad:
                    attempt_load_game();
                    break;

                case ui_component::ButtonValidate:
                    validate_fen();
                    break;

                case ui_component::ButtonCancel:
                    m_manager->pop_state();
                    break;

                case ui_component::FenBrowse:
                    open_file_dialog();
                    break;

                default:
                    break;
            }

            return;  // Found a match, no need to check further
        }
    }
}

void load_game_state::handle_text_input(const sf::Event::TextEntered& text)
{
    if (m_current_method == load_method::Direct && m_components[static_cast<size_t>(ui_component::FenInput)].active &&
        text.unicode >= 32 && text.unicode < 128) {

        if (m_text_selected) {
            m_fen_string.erase(m_selection_start, m_selection_end - m_selection_start);
            m_cursor_position = m_selection_start;
            m_text_selected = false;
        }

        m_fen_string.insert(m_cursor_position, 1, static_cast<char>(text.unicode));
        m_cursor_position++;
        m_components[static_cast<size_t>(ui_component::FenInput)].text.setString(m_fen_string);
    }
}

void load_game_state::handle_key_pressed(const sf::Event::KeyPressed& key)
{
    if (key.code == sf::Keyboard::Key::Escape) {
        m_manager->pop_state();
        return;
    }

    if (m_components[static_cast<size_t>(ui_component::FenInput)].active) {
        if (key.code == sf::Keyboard::Key::Backspace) {
            if (m_text_selected) {
                // Delete selected text
                m_fen_string.erase(m_selection_start, m_selection_end - m_selection_start);
                m_cursor_position = m_selection_start;
                m_text_selected = false;
                m_components[static_cast<size_t>(ui_component::FenInput)].text.setString(m_fen_string);
            } else if (!m_fen_string.empty() && m_cursor_position > 0) {
                // Delete character before cursor
                m_fen_string.erase(m_cursor_position - 1, 1);
                m_cursor_position--;
                m_components[static_cast<size_t>(ui_component::FenInput)].text.setString(m_fen_string);
            }
        } else if (key.code == sf::Keyboard::Key::Delete) {
            if (m_text_selected) {
                // Delete selected text
                m_fen_string.erase(m_selection_start, m_selection_end - m_selection_start);
                m_cursor_position = m_selection_start;
                m_text_selected = false;
                m_components[static_cast<size_t>(ui_component::FenInput)].text.setString(m_fen_string);
            } else if (m_cursor_position < m_fen_string.length()) {
                // Delete character at cursor
                m_fen_string.erase(m_cursor_position, 1);
                m_components[static_cast<size_t>(ui_component::FenInput)].text.setString(m_fen_string);
            }
        } else if (key.code == sf::Keyboard::Key::Left && m_cursor_position > 0) {
            m_cursor_position--;
        } else if (key.code == sf::Keyboard::Key::Right && m_cursor_position < m_fen_string.length()) {
            m_cursor_position++;
        } else if (key.code == sf::Keyboard::Key::Home) {
            m_cursor_position = 0;
        } else if (key.code == sf::Keyboard::Key::End) {
            m_cursor_position = m_fen_string.length();
        } else if (key.code == sf::Keyboard::Key::Enter) {
            validate_fen();
        }
    }
}

void load_game_state::attempt_load_game()
{
    if (!m_fen_valid) {
        m_validation_message = "Cannot load: FEN string is invalid!";
        return;
    }

    board_t    new_board;
    game_state new_state;

    auto error = fen::parse_fen(m_fen_string, new_board, new_state);
    if (error) {
        m_validation_message = *error;
        return;
    }

    if (m_player_choice == player_choice::White) {
        new_state.set_player_turn(game_state::player_turn::White);
    } else {
        new_state.set_player_turn(game_state::player_turn::Black);
    }

    m_manager->replace_state<play_state>(m_window, new_board, new_state);
}

void load_game_state::validate_fen()
{
    // Create temporary objects for validation
    board_t    test_board;
    game_state test_state;

    // Try to parse the FEN string
    auto error = fen::parse_fen(m_fen_string, test_board, test_state);

    if (!error) {
        m_fen_valid = true;
        m_validation_message = "FEN is valid!";
        m_preview_board = test_board;
        m_preview_state = test_state;
    } else {
        m_fen_valid = false;
        m_validation_message = *error;
    }
}

bool load_game_state::load_fen_from_file(const std::string& filename)
{
    try {
        // Check if file exists
        if (!std::filesystem::exists(filename)) {
            m_validation_message = "Error: File does not exist";
            return false;
        }

        // Open the file
        std::ifstream file(filename);
        if (!file.is_open()) {
            m_validation_message = "Error: Could not open file";
            return false;
        }

        // Read the FEN string
        std::string fen;
        std::getline(file, fen);

        // Check if the FEN string is valid
        if (!fen::validate_board_section(fen)) {
            m_validation_message = "Error: File does not contain a valid FEN string";
            return false;
        }

        // Set the FEN string
        m_fen_string = fen;

        return true;
    } catch (const std::exception& e) {
        m_validation_message = std::format("Error loading file: {}", e.what());
        return false;
    }
}

void load_game_state::open_file_dialog()
{
    // In a real application, you would use a native file dialog here
    // For this example, we'll simulate a file dialog with a fixed result
    m_dialog_result = "chess_position.fen";

    // For a real implementation, you could use a library like nativefiledialog or ImGui
    // Or implement your own file selection UI
}

void load_game_state::update_preview()
{
    // This would be used to show a preview of the board
    // For this implementation, we're just validating the FEN
    validate_fen();
}

}  // namespace chessfml
