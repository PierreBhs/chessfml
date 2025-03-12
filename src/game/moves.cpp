#include "game/moves.hpp"

#include "common/config.hpp"
#include "common/utils.hpp"

#include <algorithm>
#include <array>

namespace {

constexpr std::array<std::pair<int, int>, 8> knight_offsets = {
    {{-2, -1}, {-2, 1}, {-1, -2}, {-1, 2}, {1, -2}, {1, 2}, {2, -1}, {2, 1}}};

constexpr std::array<std::pair<int, int>, 8> king_offsets = {
    {{-1, -1}, {-1, 0}, {-1, 1}, {0, -1}, {0, 1}, {1, -1}, {1, 0}, {1, 1}}};

constexpr std::array<std::pair<int, int>, 4> rook_directions = {{{-1, 0}, {0, -1}, {0, 1}, {1, 0}}};

constexpr std::array<std::pair<int, int>, 4> bishop_directions = {{{-1, -1}, {-1, 1}, {1, -1}, {1, 1}}};

std::vector<chessfml::move_info> get_sliding_piece_moves(const chessfml::board_t&             board,
                                                         chessfml::move_t                     pos,
                                                         std::span<const std::pair<int, int>> directions)
{
    using namespace chessfml;

    std::vector<move_info> moves;
    const auto&            piece = board[pos];
    const auto [rank, file] = position_to_rank_file(pos);

    constexpr int board_size = config::board::size;

    for (const auto& [dr, df] : directions) {
        for (int i = 1; i < board_size; ++i) {
            int new_rank = rank + dr * i;
            int new_file = file + df * i;

            if (new_rank < 0 || new_rank >= board_size || new_file < 0 || new_file >= board_size) {
                break;
            }

            move_t      new_pos = rank_file_to_position(new_rank, new_file);
            const auto& target = board[new_pos];

            if (target.get_type() == piece_t::type_t::Empty) {
                // Empty square - valid move
                moves.push_back({.from = pos, .to = new_pos});
            } else {
                // Hit a piece
                if (target.get_color() != piece.get_color()) {
                    // Enemy piece - can capture
                    moves.push_back({.from = pos, .to = new_pos, .type = move_type_flag::Capture});
                }
                break;  // Stop in this direction after hitting a piece
            }
        }
    }

    return moves;
}

}  // anonymous namespace

namespace chessfml {

std::vector<move_info> move_generator::get_legal_moves(const board_t& board, const game_state& state, move_t pos)
{
    auto moves = get_pseudo_legal_moves(board, state, pos);

    const auto& piece = board[pos];
    const auto  player = (piece.get_color() == piece_t::color_t::White) ? game_state::player_turn::White
                                                                        : game_state::player_turn::Black;

    // Filter out moves that would leave or put the king in check
    std::erase_if(moves, [&piece, &player, &board](const move_info& move) {
        board_t temp_board = board;

        if (move.type == move_type_flag::EnPassant) {
            const auto [capture_rank, capture_file] = position_to_rank_file(move.to);
            const int    ep_rank = piece.get_color() == piece_t::color_t::White ? capture_rank + 1 : capture_rank - 1;
            const move_t ep_pos = rank_file_to_position(ep_rank, capture_file);
            temp_board[ep_pos] = piece_t{};  // Empty the captured pawn's square
        }

        if (move.type == move_type_flag::Castling) {
            const bool is_kingside = move.to > move.from;

            if (is_kingside) {
                const move_t rook_from = (player == game_state::player_turn::White) ? 7 : 63;
                const move_t rook_to = (player == game_state::player_turn::White) ? 5 : 61;

                temp_board[rook_to] = temp_board[rook_from];
                temp_board[rook_to].set_pos(rook_to);
                temp_board[rook_from] = piece_t{};
            } else {
                const move_t rook_from = (player == game_state::player_turn::White) ? 0 : 56;
                const move_t rook_to = (player == game_state::player_turn::White) ? 3 : 59;

                temp_board[rook_to] = temp_board[rook_from];
                temp_board[rook_to].set_pos(rook_to);
                temp_board[rook_from] = piece_t{};
            }
        }

        // Execute the move on the temporary board
        temp_board[move.to] = temp_board[move.from];
        temp_board[move.to].set_pos(move.to);
        temp_board[move.from] = piece_t{};

        // Check if the king would be in check after this move
        return is_in_check(temp_board, player);
    });

    return moves;
}

bool move_generator::is_legal_move(const board_t& board, const game_state& state, move_t from, move_t to)
{
    const auto legal_moves = get_legal_moves(board, state, from);

    // Check if the destination 'to' is in the list of legal moves
    return std::ranges::any_of(legal_moves, [to](const auto& move) { return move.to == to; });
}

std::vector<move_info> move_generator::get_pseudo_legal_moves(const board_t& board, const game_state& state, move_t pos)
{
    const auto& piece = board[pos];

    switch (piece.get_type()) {
        case piece_t::type_t::Pawn:
            return get_pawn_moves(board, state, pos);
        case piece_t::type_t::Rook:
            return get_rook_moves(board, pos);
        case piece_t::type_t::Knight:
            return get_knight_moves(board, pos);
        case piece_t::type_t::Bishop:
            return get_bishop_moves(board, pos);
        case piece_t::type_t::Queen:
            return get_queen_moves(board, pos);
        case piece_t::type_t::King:
            return get_king_moves(board, state, pos);
        default:
            return {};
    }
}

bool move_generator::is_in_check(const board_t& board, game_state::player_turn player)
{
    const auto king_color =
        (player == game_state::player_turn::White) ? piece_t::color_t::White : piece_t::color_t::Black;

    move_t king_pos = 0xFF;  // Invalid position

    for (move_t i = 0; i < config::board::size * config::board::size; ++i) {
        const auto& piece = board[i];
        if (piece.get_type() == piece_t::type_t::King && piece.get_color() == king_color) {
            king_pos = i;
            break;
        }
    }

    if (king_pos == 0xFF) {
        // King not found (shouldn't happen in a valid board)
        return false;
    }

    // Check if the king is attacked
    return is_square_attacked(
        board,
        king_pos,
        (player == game_state::player_turn::White) ? game_state::player_turn::Black : game_state::player_turn::White);
}

bool move_generator::is_checkmate(const board_t& board, const game_state& state)
{
    const auto player_turn = state.get_player_turn();

    return is_in_check(board, player_turn) && !has_legal_moves(board, state);
}

bool move_generator::is_stalemate(const board_t& board, const game_state& state)
{
    const auto player_turn = state.get_player_turn();

    return !is_in_check(board, player_turn) && !has_legal_moves(board, state);
}

std::vector<move_info> move_generator::get_pawn_moves(const board_t& board, const game_state& state, move_t pos)
{
    std::vector<move_info> moves;
    const auto&            pawn = board[pos];

    if (pawn.get_type() != piece_t::type_t::Pawn) {
        return moves;
    }

    const bool is_white = (pawn.get_color() == piece_t::color_t::White);
    const int  direction = is_white ? -1 : 1;
    const auto [rank, file] = position_to_rank_file(pos);

    constexpr int board_size = config::board::size;

    // Check if we're at the last rank (shouldn't happen in a valid board, since you promote)
    if ((is_white && rank == 0) || (!is_white && rank == board_size - 1)) {
        return moves;
    }

    // Single step forward
    const int new_rank = rank + direction;
    if (new_rank >= 0 && new_rank < board_size) {
        const move_t forward_pos = rank_file_to_position(new_rank, file);
        if (board[forward_pos].get_type() == piece_t::type_t::Empty) {
            // Check for promotion
            if ((is_white && new_rank == 0) || (!is_white && new_rank == board_size - 1)) {
                for (auto type : {piece_t::type_t::Queen,
                                  piece_t::type_t::Rook,
                                  piece_t::type_t::Bishop,
                                  piece_t::type_t::Knight}) {
                    moves.push_back({.from = pos,
                                     .to = forward_pos,
                                     .type = move_type_flag::Promotion,
                                     .promotion_piece = static_cast<std::uint8_t>(type)});
                }
            } else {
                // Normal move
                moves.push_back({.from = pos, .to = forward_pos});

                // Double step from starting position
                if (!pawn.has_moved()) {
                    const int double_rank = rank + 2 * direction;
                    if (double_rank >= 0 && double_rank < board_size) {
                        const move_t double_pos = rank_file_to_position(double_rank, file);
                        if (board[double_pos].get_type() == piece_t::type_t::Empty) {
                            moves.push_back({.from = pos, .to = double_pos});
                        }
                    }
                }
            }
        }
    }

    // Captures (including en passant)
    for (int df : {-1, 1}) {
        if (file + df < 0 || file + df >= board_size)
            continue;

        const int    new_file = file + df;
        const move_t capture_pos = rank_file_to_position(new_rank, new_file);

        // Regular capture
        if (new_rank >= 0 && new_rank < board_size) {
            const auto& target = board[capture_pos];
            if (target.get_type() != piece_t::type_t::Empty && target.get_color() != pawn.get_color()) {

                // Check for capture with promotion
                if ((is_white && new_rank == 0) || (!is_white && new_rank == board_size - 1)) {
                    for (auto type : {piece_t::type_t::Queen,
                                      piece_t::type_t::Rook,
                                      piece_t::type_t::Bishop,
                                      piece_t::type_t::Knight}) {
                        moves.push_back({.from = pos,
                                         .to = capture_pos,
                                         .type = move_type_flag::Capture | move_type_flag::Promotion,
                                         .promotion_piece = static_cast<std::uint8_t>(type)});
                    }
                } else {
                    moves.push_back({.from = pos, .to = capture_pos, .type = move_type_flag::Capture});
                }
            }

            // En passant capture
            if (state.get_en_passant_target().has_value() && capture_pos == state.get_en_passant_target().value()) {
                moves.push_back(
                    {.from = pos, .to = capture_pos, .type = move_type_flag::Capture | move_type_flag::EnPassant});
            }
        }
    }

    return moves;
}

std::vector<move_info> move_generator::get_rook_moves(const board_t& board, move_t pos)
{
    return get_sliding_piece_moves(board, pos, rook_directions);
}

std::vector<move_info> move_generator::get_knight_moves(const board_t& board, move_t pos)
{
    std::vector<move_info> moves;
    const auto&            knight = board[pos];
    const auto [rank, file] = position_to_rank_file(pos);

    constexpr int board_size = config::board::size;

    for (const auto& [dr, df] : knight_offsets) {
        int new_rank = rank + dr;
        int new_file = file + df;

        if (new_rank < 0 || new_rank >= board_size || new_file < 0 || new_file >= board_size) {
            continue;
        }

        move_t      new_pos = rank_file_to_position(new_rank, new_file);
        const auto& target = board[new_pos];

        if (target.get_type() == piece_t::type_t::Empty) {
            moves.push_back({.from = pos, .to = new_pos});
        } else if (target.get_color() != knight.get_color()) {
            moves.push_back({.from = pos, .to = new_pos, .type = move_type_flag::Capture});
        }
    }

    return moves;
}

std::vector<move_info> move_generator::get_bishop_moves(const board_t& board, move_t pos)
{
    return get_sliding_piece_moves(board, pos, bishop_directions);
}

std::vector<move_info> move_generator::get_queen_moves(const board_t& board, move_t pos)
{
    // Queen moves are a combination of rook and bishop moves
    const auto rook_moves = get_rook_moves(board, pos);
    const auto bishop_moves = get_bishop_moves(board, pos);

    std::vector<move_info> queen_moves;
    queen_moves.reserve(rook_moves.size() + bishop_moves.size());

    queen_moves.insert(
        queen_moves.end(), std::make_move_iterator(rook_moves.begin()), std::make_move_iterator(rook_moves.end()));
    queen_moves.insert(
        queen_moves.end(), std::make_move_iterator(bishop_moves.begin()), std::make_move_iterator(bishop_moves.end()));

    return queen_moves;
}

std::vector<move_info> move_generator::get_king_moves(const board_t& board, const game_state& state, move_t pos)
{
    std::vector<move_info> moves;
    const auto&            king = board[pos];
    const auto [rank, file] = position_to_rank_file(pos);
    const auto king_color = king.get_color();

    // Convert piece color to player turn for state queries
    const auto player =
        (king_color == piece_t::color_t::White) ? game_state::player_turn::White : game_state::player_turn::Black;

    constexpr int board_size = config::board::size;

    for (const auto& [dr, df] : king_offsets) {
        int new_rank = rank + dr;
        int new_file = file + df;

        if (new_rank < 0 || new_rank >= board_size || new_file < 0 || new_file >= board_size) {
            continue;
        }

        move_t      new_pos = rank_file_to_position(new_rank, new_file);
        const auto& target = board[new_pos];

        if (is_square_attacked(board,
                               new_pos,
                               (player == game_state::player_turn::White) ? game_state::player_turn::Black
                                                                          : game_state::player_turn::White)) {
            continue;
        }

        if (target.get_type() == piece_t::type_t::Empty) {
            moves.push_back({.from = pos, .to = new_pos});
        } else if (target.get_color() != king.get_color()) {
            moves.push_back({.from = pos, .to = new_pos, .type = move_type_flag::Capture});
        }
    }

    // Castling moves
    if (!king.has_moved() && !state.is_check()) {
        if (state.can_castle_kingside(player)) {
            const move_t rook_pos = (king_color == piece_t::color_t::White) ? 7 : 63;
            const move_t king_path[] = {static_cast<move_t>(pos + 1), static_cast<move_t>(pos + 2)};

            bool path_clear = true;

            // Check if squares between king and rook are empty
            for (int i = 0; i < 2; ++i) {
                if (board[king_path[i]].get_type() != piece_t::type_t::Empty) {
                    path_clear = false;
                    break;
                }
            }

            // Check if squares the king moves through are not under attack
            if (path_clear) {
                for (int i = 0; i < 2; ++i) {
                    if (is_square_attacked(board,
                                           king_path[i],
                                           (player == game_state::player_turn::White)
                                               ? game_state::player_turn::Black
                                               : game_state::player_turn::White)) {
                        path_clear = false;
                        break;
                    }
                }
            }

            // Verify the rook is there and hasn't moved
            if (path_clear && board[rook_pos].get_type() == piece_t::type_t::Rook &&
                board[rook_pos].get_color() == king_color && !board[rook_pos].has_moved()) {

                moves.push_back({.from = pos, .to = static_cast<move_t>(pos + 2), .type = move_type_flag::Castling});
            }
        }

        // Queenside castling
        if (state.can_castle_queenside(player)) {
            const move_t rook_pos = (king_color == piece_t::color_t::White) ? 0 : 56;
            const move_t king_path[] = {static_cast<move_t>(pos - 1), static_cast<move_t>(pos - 2)};
            const move_t full_path[] = {
                static_cast<move_t>(pos - 1), static_cast<move_t>(pos - 2), static_cast<move_t>(pos - 3)};

            bool path_clear = true;

            // Check if squares between king and rook are empty
            for (int i = 0; i < 3; ++i) {
                if (board[full_path[i]].get_type() != piece_t::type_t::Empty) {
                    path_clear = false;
                    break;
                }
            }

            // Check if squares the king moves through are not under attack
            if (path_clear) {
                for (int i = 0; i < 2; ++i) {  // Only check the squares the king moves through
                    if (is_square_attacked(board,
                                           king_path[i],
                                           (player == game_state::player_turn::White)
                                               ? game_state::player_turn::Black
                                               : game_state::player_turn::White)) {
                        path_clear = false;
                        break;
                    }
                }
            }

            // Verify the rook is there and hasn't moved
            if (path_clear && board[rook_pos].get_type() == piece_t::type_t::Rook &&
                board[rook_pos].get_color() == king_color && !board[rook_pos].has_moved()) {

                moves.push_back({.from = pos, .to = static_cast<move_t>(pos - 2), .type = move_type_flag::Castling});
            }
        }
    }

    return moves;
}

bool move_generator::is_square_attacked(const board_t& board, move_t square, game_state::player_turn attacker_turn)
{
    const auto attacker_color =
        (attacker_turn == game_state::player_turn::White) ? piece_t::color_t::White : piece_t::color_t::Black;

    const auto [sq_rank, sq_file] = position_to_rank_file(square);

    constexpr int board_size = config::board::size;

    // Check pawn attacks
    const int pawn_dir = (attacker_color == piece_t::color_t::White) ? -1 : 1;
    for (int df : {-1, 1}) {
        int pawn_rank = sq_rank - pawn_dir;
        int pawn_file = sq_file + df;

        if (pawn_rank < 0 || pawn_rank >= board_size || pawn_file < 0 || pawn_file >= board_size) {
            continue;
        }

        move_t      pawn_pos = rank_file_to_position(pawn_rank, pawn_file);
        const auto& piece = board[pawn_pos];

        if (piece.get_type() == piece_t::type_t::Pawn && piece.get_color() == attacker_color) {
            return true;
        }
    }

    // Check knight attacks
    for (const auto& [dr, df] : knight_offsets) {
        int knight_rank = sq_rank + dr;
        int knight_file = sq_file + df;

        if (knight_rank < 0 || knight_rank >= board_size || knight_file < 0 || knight_file >= board_size) {
            continue;
        }

        move_t      knight_pos = rank_file_to_position(knight_rank, knight_file);
        const auto& piece = board[knight_pos];

        if (piece.get_type() == piece_t::type_t::Knight && piece.get_color() == attacker_color) {
            return true;
        }
    }

    // Check king attacks (adjacent squares)
    for (const auto& [dr, df] : king_offsets) {
        int king_rank = sq_rank + dr;
        int king_file = sq_file + df;

        if (king_rank < 0 || king_rank >= board_size || king_file < 0 || king_file >= board_size) {
            continue;
        }

        move_t      king_pos = rank_file_to_position(king_rank, king_file);
        const auto& piece = board[king_pos];

        if (piece.get_type() == piece_t::type_t::King && piece.get_color() == attacker_color) {
            return true;
        }
    }

    // Check rook and queen attacks (horizontal and vertical)
    for (const auto& [dr, df] : rook_directions) {
        for (int i = 1; i < board_size; ++i) {
            int r = sq_rank + dr * i;
            int f = sq_file + df * i;

            if (r < 0 || r >= board_size || f < 0 || f >= board_size) {
                break;
            }

            move_t      pos = rank_file_to_position(r, f);
            const auto& piece = board[pos];

            if (piece.get_type() != piece_t::type_t::Empty) {
                if ((piece.get_type() == piece_t::type_t::Rook || piece.get_type() == piece_t::type_t::Queen) &&
                    piece.get_color() == attacker_color) {
                    return true;
                }
                break;  // Blocked by another piece
            }
        }
    }

    // Check bishop and queen attacks (diagonal)
    for (const auto& [dr, df] : bishop_directions) {
        for (int i = 1; i < board_size; ++i) {
            int r = sq_rank + dr * i;
            int f = sq_file + df * i;

            if (r < 0 || r >= board_size || f < 0 || f >= board_size) {
                break;
            }

            move_t      pos = rank_file_to_position(r, f);
            const auto& piece = board[pos];

            if (piece.get_type() != piece_t::type_t::Empty) {
                if ((piece.get_type() == piece_t::type_t::Bishop || piece.get_type() == piece_t::type_t::Queen) &&
                    piece.get_color() == attacker_color) {
                    return true;
                }
                break;  // Blocked by another piece
            }
        }
    }

    return false;
}

bool move_generator::has_legal_moves(const board_t& board, const game_state& state)
{
    const auto player_turn = state.get_player_turn();
    const auto piece_color =
        (player_turn == game_state::player_turn::White) ? piece_t::color_t::White : piece_t::color_t::Black;

    for (move_t pos = 0; pos < config::board::size * config::board::size; ++pos) {
        const auto& piece = board[pos];
        if (piece.get_type() != piece_t::type_t::Empty && piece.get_color() == piece_color) {
            auto legal_moves = get_legal_moves(board, state, pos);
            if (!legal_moves.empty()) {
                return true;
            }
        }
    }

    return false;
}

}  // namespace chessfml
