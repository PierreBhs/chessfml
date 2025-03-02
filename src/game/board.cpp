#include "game/board.hpp"

#include "common/config.hpp"

namespace chessfml {

std::expected<std::string_view, fen_error> board_t::fen_to_board(std::string_view fen)
{
    std::uint8_t index{0u};
    for (const auto c : fen) {
        if (index >= 64) {
            break;
        }

        if (c == '/') {
            continue;
        }

        if (std::isdigit(c)) {
            auto empty_squares{0};
            auto [ptr, ec] = std::from_chars(&c, &c + 1, empty_squares);
            if (ec != std::errc() || empty_squares < 1 || empty_squares > 8) {
                return std::unexpected(fen_error::InvalidCharacter);
            }

            while (empty_squares-- > 0) {
                if (index >= 64)
                    return std::unexpected(fen_error::InvalidFormat);
                m_board[index] = piece_t{piece_t::type_t::Empty, index};
                index++;
            }
        } else {
            const auto pc = std::isupper(c) ? piece_t::color_t::White : piece_t::color_t::Black;
            const auto piece_char = std::toupper(c);

            switch (piece_char) {
                case 'P':
                    m_board[index] = piece_t{piece_t::type_t::Pawn, index, pc};
                    break;
                case 'R':
                    m_board[index] = piece_t{piece_t::type_t::Rook, index, pc};
                    break;
                case 'N':
                    m_board[index] = piece_t{piece_t::type_t::Knight, index, pc};
                    break;
                case 'B':
                    m_board[index] = piece_t{piece_t::type_t::Bishop, index, pc};
                    break;
                case 'Q':
                    m_board[index] = piece_t{piece_t::type_t::Queen, index, pc};
                    break;
                case 'K':
                    m_board[index] = piece_t{piece_t::type_t::King, index, pc};
                    break;
                default:
                    m_board[index] = piece_t{piece_t::type_t::Empty, index, pc};
                    // return std::unexpected(fen_error::InvalidCharacter);
            }
            ++index;
        }
    }

    if (index != 64) {
        return std::unexpected(fen_error::InvalidFormat);
    }

    return "";
}

void board_t::set_board_fen(std::string_view fen)
{
    fen_to_board(fen).value_or(config::board::fen_starting_position);
}

void board_t::print() const
{
    auto row{0};
    for (const auto& piece : m_board) {
        std::print("{}", piece.c_type());
        if (++row == 8) {
            std::println();
            row = 0;
        }
    }
}

}  // namespace chessfml
