#pragma once

namespace chessfml {

class game_state
{
public:
    enum class state { MainMenu, Playing, EndScreen };
    enum class player_turn { White, Black };

    const state& get_state() const { return m_state; }
    void         set_state(state state) { m_state = state; }

    const player_turn& get_player_turn() const { return m_turn; }
    void next_turn() { m_turn = (m_turn == player_turn::White) ? player_turn::Black : player_turn::White; }

private:
    state       m_state;
    player_turn m_turn{};
};

}  // namespace chessfml
