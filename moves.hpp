// move_logic.hpp
#ifndef move_logic_hpp
#define move_logic_hpp

#include "board_t.hpp"
#include "move_t.hpp"
#include <array>
#include <vector>

piece_t make_move(board_t& board, const move_t& move) {
    square_t& from_square = board.at(move.from_x, move.from_y);
    square_t& to_square   = board.at(move.to_x, move.to_y);

    // Save captured piece before overwriting
    piece_t captured_piece = to_square.piece;

    to_square.piece   = from_square.piece;
    from_square.piece = piece_t{};

    if (move.promotion_type != PieceType::EMPTY) {
        to_square.piece.type = move.promotion_type;
    }

    return captured_piece;
}

void undo_move(board_t& board, const move_t& move, const piece_t& captured_piece) {
    square_t& from_square = board.at(move.from_x, move.from_y);
    square_t& to_square   = board.at(move.to_x, move.to_y);

    // Move the piece back
    from_square.piece = to_square.piece;

    // If it was a promotion, restore the original piece type (PAWN)
    if (move.promotion_type != PieceType::EMPTY) {
        from_square.piece.type = PieceType::PAWN;
    }

    // Restore the captured piece
    to_square.piece = captured_piece;
}

vector<square_t> line(const board_t& board, const move_t& move) {
    vector<square_t> squares;

    bool up    = move.from_y < move.to_y;
    bool right = move.from_x < move.to_x;

    if (move.from_x == move.to_x) {
        // Vertical line
        if (move.from_y < move.to_y) {
            // Vertical line going up
            for (int j = move.from_y + 1; j < move.to_y; j++) {
                squares.push_back(board.at(move.from_x, j));
            }
            return squares;
        } else {
            // Vertical line going down
            for (int j = move.from_y - 1; j > move.to_y; j--) {
                squares.push_back(board.at(move.from_x, j));
            }
            return squares;
        }
    } else if (move.from_y == move.to_y) {
        // Horizontal line
        if (move.from_x < move.to_x) {
            // Horizontal line going right
            for (int i = move.from_x + 1; i < move.to_x; i++) {
                squares.push_back(board.at(i, move.from_y));
            }
            return squares;
        } else {
            // Horizontal line going left
            for (int i = move.from_x - 1; i > move.to_x; i--) {
                squares.push_back(board.at(i, move.from_y));
            }
            return squares;
        }
    }

    // x and y increment based on the direction of the line
    int x_dir = 2 * right - 1;
    int y_dir = 2 * up - 1;
    // Diagonals
    for (int i = 1; i < abs(move.to_x - move.from_x); i++) {
        squares.push_back(board.at(move.from_x + x_dir * i, move.from_y + y_dir * i));
    }
    return squares;
}

// returns true if there's a threatening piece in this line that can give check
bool is_check_from_line(const board_t& board, const vector<square_t>& diagonal, Color color, bool is_diagonal) {
    for (const auto& square : diagonal) {
        const piece_t& piece = board.at(square.x, square.y).piece;

        if (piece.type != PieceType::EMPTY) {
            // check if it's an enemy bishop/queen on diagonal, or rook/queen on straight line
            if (piece.color != color && ((is_diagonal && (piece.type == PieceType::BISHOP || piece.type == PieceType::QUEEN)) ||
                                         (!is_diagonal && (piece.type == PieceType::ROOK || piece.type == PieceType::QUEEN)))) {
                return true;
            }
            // if we hit any other piece, it blocks line of sight
            return false;
        }
    }
    return false;
}

bool is_in_check(const board_t& board, Color color) {
    // find the king
    int k_x = -1, k_y = -1;
    for (const auto& square : board) {
        if (square.piece.type == PieceType::KING && square.piece.color == color) {
            k_x = square.x;
            k_y = square.y;
            break;
        }
    }
    if (k_x == -1)
        return false; // no king found

    // check for knights
    const vector<array<int, 2>> knight_moves = {
        {k_x + 1, k_y + 2}, {k_x - 1, k_y + 2}, {k_x + 1, k_y - 2}, {k_x - 1, k_y - 2}, {k_x + 2, k_y + 1}, {k_x - 2, k_y + 1}, {k_x + 2, k_y - 1}, {k_x - 2, k_y - 1}};
    for (const auto& [x, y] : knight_moves) {
        if (board.in_board(x, y) &&
            board.at(x, y).piece.type == PieceType::KNIGHT &&
            board.at(x, y).piece.color != color) {
            return true;
        }
    }

    // check diagonals for bishops and queens
    const vector<square_t> diagonal_ur = line(board, move_t{k_x, k_y, k_x + min(7 - k_x, 7 - k_y), k_y + min(7 - k_x, 7 - k_y), PieceType::EMPTY, 0});
    const vector<square_t> diagonal_ul = line(board, move_t{k_x, k_y, k_x - min(k_x, 7 - k_y), k_y + min(k_x, 7 - k_y), PieceType::EMPTY, 0});
    const vector<square_t> diagonal_dr = line(board, move_t{k_x, k_y, k_x + min(7 - k_x, k_y), k_y - min(7 - k_x, k_y), PieceType::EMPTY, 0});
    const vector<square_t> diagonal_dl = line(board, move_t{k_x, k_y, k_x - min(k_x, k_y), k_y - min(k_x, k_y), PieceType::EMPTY, 0});

    for (const auto& diagonal : array<vector<square_t>, 4>{diagonal_ur, diagonal_ul, diagonal_dr, diagonal_dl}) {
        if (is_check_from_line(board, diagonal, color, true)) {
            return true;
        }
    }

    // check straight lines for rooks and queens
    const vector<square_t> line_u = line(board, move_t{k_x, k_y, k_x, 7, PieceType::EMPTY, 0});
    const vector<square_t> line_r = line(board, move_t{k_x, k_y, 7, k_y, PieceType::EMPTY, 0});
    const vector<square_t> line_d = line(board, move_t{k_x, k_y, k_x, 0, PieceType::EMPTY, 0});
    const vector<square_t> line_l = line(board, move_t{k_x, k_y, 0, k_y, PieceType::EMPTY, 0});

    for (const auto& straight_line : array<vector<square_t>, 4>{line_u, line_r, line_d, line_l}) {
        if (is_check_from_line(board, straight_line, color, false)) {
            return true;
        }
    }

    // check for pawns
    const vector<array<int, 2>> pawn_attacks = (color == Color::WHITE) ? vector<array<int, 2>>{{k_x + 1, k_y + 1}, {k_x - 1, k_y + 1}} : vector<array<int, 2>>{{k_x + 1, k_y - 1}, {k_x - 1, k_y - 1}};

    for (const auto& [x, y] : pawn_attacks) {
        if (board.in_board(x, y) &&
            board.at(x, y).piece.type == PieceType::PAWN &&
            board.at(x, y).piece.color != color) {
            return true;
        }
    }

    return false;
}

bool is_move_legal(board_t& board, const move_t& move) {
    vector<square_t> line_of_move = line(board, move);
    if (board.at(move.from_x, move.from_y).piece.type != PieceType::KNIGHT) {
        for (auto& square : line_of_move) {
            if (board.at(square.x, square.y).piece.type != PieceType::EMPTY) {
                return false;
            }
        }
    }

    Color moving_color = board.at(move.from_x, move.from_y).piece.color;
    if (board.at(move.to_x, move.to_y).piece.color == moving_color) {
        return false;
    }

    // make move and check if it reveals check
    piece_t captured = make_move(board, move);
    bool is_checked  = is_in_check(board, moving_color);
    undo_move(board, move, captured);

    return !is_checked;
}

vector<move_t> get_pawn_moves(const board_t& board, int x, int y) {
    vector<move_t> moves;
    Color own_color = board.at(x, y).piece.color;
    int direction   = (own_color == Color::WHITE) ? 1 : -1;

    // regular move
    if (board.in_board(x, y + direction) && board.at(x, y + direction).piece.type == PieceType::EMPTY) {
        // promotion for straight moves
        if ((own_color == Color::WHITE && y + direction == 7) ||
            (own_color == Color::BLACK && y + direction == 0)) {
            for (PieceType promotion : {PieceType::QUEEN, PieceType::ROOK,
                                        PieceType::BISHOP, PieceType::KNIGHT}) {
                moves.push_back(move_t{x, y, x, y + direction, promotion, false});
            }
        } else {
            moves.push_back(move_t{x, y, x, y + direction, PieceType::EMPTY, false});

            // first move - two squares (only if not promoting)
            if ((own_color == Color::WHITE && y == 1) || (own_color == Color::BLACK && y == 6)) {
                if (board.at(x, y + 2 * direction).piece.type == PieceType::EMPTY) {
                    moves.push_back(move_t{x, y, x, y + 2 * direction, PieceType::EMPTY, false});
                }
            }
        }
    }

    // captures
    for (int dx : {-1, 1}) {
        if (board.in_board(x + dx, y + direction)) {
            const square_t& target = board.at(x + dx, y + direction);
            if (target.piece.type != PieceType::EMPTY && target.piece.color != own_color) {
                // promotion for capture moves
                if ((own_color == Color::WHITE && y + direction == 7) ||
                    (own_color == Color::BLACK && y + direction == 0)) {
                    for (PieceType promotion : {PieceType::QUEEN, PieceType::ROOK,
                                                PieceType::BISHOP, PieceType::KNIGHT}) {
                        moves.push_back(move_t{x, y, x + dx, y + direction, promotion, false});
                    }
                } else {
                    moves.push_back(move_t{x, y, x + dx, y + direction, PieceType::EMPTY, false});
                }
            }
        }
    }

    // TODO: en passant

    return moves;
}

vector<move_t> get_rook_moves(const board_t& board, int x, int y) {
    vector<move_t> moves;
    Color own_color = board.at(x, y).piece.color;

    const array<pair<int, int>, 4> directions = {
        make_pair(0, 1),  // up
        make_pair(0, -1), // down
        make_pair(1, 0),  // right
        make_pair(-1, 0)  // left
    };

    for (const auto& [dx, dy] : directions) {
        int current_x = x + dx;
        int current_y = y + dy;

        while (board.in_board(current_x, current_y)) {
            const square_t& target = board.at(current_x, current_y);

            if (target.piece.type == PieceType::EMPTY) {
                moves.push_back(move_t{x, y, current_x, current_y, PieceType::EMPTY, false});
            } else {
                if (target.piece.color != own_color) {
                    moves.push_back(move_t{x, y, current_x, current_y, PieceType::EMPTY, false});
                }
                break; // stop after encountering any piece
            }

            current_x += dx;
            current_y += dy;
        }
    }

    return moves;
}

vector<move_t> get_knight_moves(const board_t& board, int x, int y) {
    vector<move_t> moves;
    Color own_color = board.at(x, y).piece.color;

    vector<array<int, 2>> offsets = {
        {x + 1, y + 2}, {x - 1, y + 2}, {x + 1, y - 2}, {x - 1, y - 2}, {x + 2, y + 1}, {x - 2, y + 1}, {x + 2, y - 1}, {x - 2, y - 1}};

    for (const auto& [to_x, to_y] : offsets) {
        if (board.in_board(to_x, to_y) && board.at(to_x, to_y).piece.color != own_color) {
            moves.push_back(move_t{x, y, to_x, to_y, PieceType::EMPTY, false});
        }
    }

    return moves;
}

vector<move_t> get_bishop_moves(const board_t& board, int x, int y) {
    vector<move_t> moves;
    Color own_color = board.at(x, y).piece.color;

    const array<pair<int, int>, 4> directions = {
        make_pair(1, 1),  // up-right
        make_pair(1, -1), // down-right
        make_pair(-1, 1), // up-left
        make_pair(-1, -1) // down-left
    };

    for (const auto& [dx, dy] : directions) {
        int current_x = x + dx;
        int current_y = y + dy;

        while (board.in_board(current_x, current_y)) {
            const square_t& target = board.at(current_x, current_y);

            if (target.piece.type == PieceType::EMPTY) {
                moves.push_back(move_t{x, y, current_x, current_y, PieceType::EMPTY, false});
            } else {
                if (target.piece.color != own_color) {
                    moves.push_back(move_t{x, y, current_x, current_y, PieceType::EMPTY, false});
                }
                break; // stop after encountering any piece
            }

            current_x += dx;
            current_y += dy;
        }
    }

    return moves;
}

vector<move_t> get_queen_moves(const board_t& board, int x, int y) {
    vector<move_t> line_moves     = get_rook_moves(board, x, y);
    vector<move_t> diagonal_moves = get_bishop_moves(board, x, y);
    line_moves.insert(line_moves.end(), diagonal_moves.begin(), diagonal_moves.end());
    return line_moves;
}

vector<move_t> get_king_moves(const board_t& board, int x, int y) {
    vector<move_t> moves;
    Color own_color = board.at(x, y).piece.color;

    vector<array<int, 2>> offsets = {
        {x + 1, y + 1}, {x + 1, y}, {x + 1, y - 1}, {x, y - 1}, {x - 1, y - 1}, {x - 1, y}, {x - 1, y + 1}, {x, y + 1}};

    for (const auto& [to_x, to_y] : offsets) {
        if (board.in_board(to_x, to_y) && board.at(to_x, to_y).piece.color != own_color) {
            moves.push_back(move_t{x, y, to_x, to_y, PieceType::EMPTY, false});
        }
    }

    // TODO: Castling moves should be added here

    return moves;
}

vector<move_t> get_piece_moves(const board_t& board, int x, int y) {
    vector<move_t> moves;
    const square_t& square = board.at(x, y);

    switch (square.piece.type) {
    case PieceType::PAWN: return get_pawn_moves(board, x, y);
    case PieceType::ROOK: return get_rook_moves(board, x, y);
    case PieceType::KNIGHT: return get_knight_moves(board, x, y);
    case PieceType::BISHOP: return get_bishop_moves(board, x, y);
    case PieceType::QUEEN: return get_queen_moves(board, x, y);
    case PieceType::KING: return get_king_moves(board, x, y);
    default: return moves;
    }
}

vector<move_t> get_all_moves(const board_t& board) {
    vector<move_t> all_moves;

    for (const auto& square : board) {
        // skip empty squares and pieces of the wrong color
        if (square.piece.type == PieceType::EMPTY) {
            continue;
        }

        vector<move_t> piece_moves = get_piece_moves(board, square.x, square.y);

        // check if each move is legal
        for (const auto& move : piece_moves) {
            board_t temp_board = board; // Create a copy of the board
            if (is_move_legal(temp_board, move)) {
                all_moves.push_back(move);
            }
        }
    }

    return all_moves;
}

vector<move_t> generate_all_moves_for_color(board_t& board, Color color) {
    vector<move_t> all_moves;

    for (const auto& square : board) {
        // skip empty squares and pieces of the wrong color
        if (square.piece.type == PieceType::EMPTY || square.piece.color != color) {
            continue;
        }

        // bounds checking
        if (!board.in_board(square.x, square.y)) {
            continue;
        }

        vector<move_t> piece_moves = get_piece_moves(board, square.x, square.y);

        for (const auto& move : piece_moves) {
            piece_t captured = make_move(board, move);
            bool is_valid    = !is_in_check(board, color);
            undo_move(board, move, captured);

            if (is_valid) {
                all_moves.push_back(move);
            }
        }
    }

    return all_moves;
}

uint64_t perft(board_t& board, int depth, Color color) {
    if (depth == 0)
        return 1;

    uint64_t nodes       = 0;
    vector<move_t> moves = generate_all_moves_for_color(board, color);

    for (const move_t& move : moves) {
        piece_t captured_piece = make_move(board, move);
        nodes += perft(board, depth - 1, color == Color::WHITE ? Color::BLACK : Color::WHITE);
        undo_move(board, move, captured_piece);
    }

    return nodes;
}

void test_perft() {
    board_t board;
    board.initialize_starting_board();

    // hardcoded expected values for perft from starting position
    const vector<uint64_t> expected_white = {
        1,       // depth 0
        20,      // depth 1
        400,     // depth 2
        8902,    // depth 3
        197281,  // depth 4
        4865609, // depth 5
    };

    const int max_depth = expected_white.size() - 1;

    for (int depth = 0; depth <= max_depth; depth++) {
        uint64_t result = perft(board, depth, Color::WHITE);
        cout << "Testing perft for white with depth of " << depth
             << ". Result: " << result
             << ", Expected: " << expected_white[depth];

        if (result == expected_white[depth]) {
            cout << " ✓" << endl;
        } else {
            cout << " ✗" << endl;
        }
    }
}

#endif