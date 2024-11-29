#ifndef operator_overload_util_hpp
#define operator_overload_util_hpp

#include "board_t.hpp"
#include "piece_t.hpp"
#include "square_t.hpp"
#include <iostream>


inline std::ostream& operator<<(std::ostream& os, square_t square) {
    if (square.piece.color == Color::NONE) {
        return os << ((square.x + square.y) % 2 == 0 ? "·" : "·");
    }
    if (square.piece.color == Color::WHITE) {
        switch (square.piece.type) {
        case PieceType::KING:
            return os << "\u265A";
        case PieceType::QUEEN:
            return os << "\u265B";
        case PieceType::ROOK:
            return os << "\u265C";
        case PieceType::BISHOP:
            return os << "\u265D";
        case PieceType::KNIGHT:
            return os << "\u265E";
        case PieceType::PAWN:
            return os << "\u265F";
        default:
            return os << " ";
        }
    } else {
        switch (square.piece.type) {
        case PieceType::KING:
            return os << "\u2654";
        case PieceType::QUEEN:
            return os << "\u2655";
        case PieceType::ROOK:
            return os << "\u2656";
        case PieceType::BISHOP:
            return os << "\u2657";
        case PieceType::KNIGHT:
            return os << "\u2658";
        case PieceType::PAWN:
            return os << "\u2659";
        default:
            return os << " ";
        }
    }
}


// The ASCII version of the above

// inline std::ostream& operator<<(std::ostream& os, square_t square) {
//     if (square.piece.color == Color::NONE) {
//         return os << ((square.x + square.y) % 2 == 0 ? "O " : "  ");
//     }
//     if (square.piece.color == Color::WHITE) {
//         switch (square.piece.type) {
//         case PieceType::KING:
//             return os << "wK";
//         case PieceType::QUEEN:
//             return os << "wQ";
//         case PieceType::ROOK:
//             return os << "wR";
//         case PieceType::BISHOP:
//             return os << "wB";
//         case PieceType::KNIGHT:
//             return os << "wN";
//         case PieceType::PAWN:
//             return os << "wP";
//         default:
//             return os << "  ";
//         }
//     } else { // BLACK
//         switch (square.piece.type) {
//         case PieceType::KING:
//             return os << "bK";
//         case PieceType::QUEEN:
//             return os << "bQ";
//         case PieceType::ROOK:
//             return os << "bR";
//         case PieceType::BISHOP:
//             return os << "bB";
//         case PieceType::KNIGHT:
//             return os << "bN";
//         case PieceType::PAWN:
//             return os << "bP";
//         default:
//             return os << "  ";
//         }
//     }
// }

#endif