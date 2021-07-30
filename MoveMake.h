//
// Created by evcmo on 7/4/2021.
//

#pragma once
#ifndef CHARON_MOVEMAKE_H
#define CHARON_MOVEMAKE_H

#include "ChaosMagic.h"
#include "Board.h"
#include "Move.h"
#include <cassert>

namespace Charon {

    enum CheckType : uint8_t {
        None,
        Check,
        DoubleCheck
    };

    struct MoveWrap final {
        Move move;
        explicit operator Move() const
        { return move; }
        MoveWrap& operator=(Move m)
        { move = m; return *this; }
        operator float() const = delete;
    };

    constexpr CheckType
    calculateCheck(uint64_t checkBoard) {
        return !checkBoard ? None:
               (checkBoard & (checkBoard - 1)) ?
               DoubleCheck: Check;
    }

    /**
     * TODO: MOVE BACK TO ANON NAMESPACE IN CPP !!!
     *
     * A function to calculate attacks on the fly.
     *
     * @tparam A    the alliance of the piece
     *              on the square under attack
     * @tparam PT   the piece type
     * @param board the current game board
     * @param sq    the square under attack
     */
    template<Alliance A, PieceType PT> [[nodiscard]]
    constexpr uint64_t attacksOn(Board* const board, const int sq) {
        static_assert(A == White || A == Black);
        static_assert(PT >= Pawn && PT <= NullPT);

        // Determine alliances.
        constexpr const Alliance us = A, them = ~us;
        // Get their queen.

        const uint64_t theirQueens = board->getPieces<them, Queen>(),
                       target      = board->getPieces<us, PT>(),
        // Board minus king
                       allPieces   = board->getAllPieces() & ~target;
        // Get the attack board for horizontal and vertical attacks
        // on the given square.
        const uint64_t onAxisAttackMask   = attackBoard<Rook>(allPieces, sq);
        // Get the attack board for diagonal attacks on the given
        // square.
        const uint64_t diagonalAttackMask = attackBoard<Bishop>(allPieces, sq);

        // Calculate and return a bitboard representing all attackers.
        return PT == King ?
               (onAxisAttackMask &
                (board->getPieces<them, Rook>()   | theirQueens)) |
               (diagonalAttackMask &
                (board->getPieces<them, Bishop>() | theirQueens)) |
               (attackBoard<Knight>(sq) & board->getPieces<them, Knight>()) |
               (attackBoard<us, Pawn>(sq) & board->getPieces<them, Pawn>())
                          :
               (onAxisAttackMask &
                (board->getPieces<them, Rook>()   | theirQueens)) |
               (diagonalAttackMask &
                (board->getPieces<them, Bishop>() | theirQueens)) |
               (attackBoard<Knight>(sq) & board->getPieces<them, Knight>()) |
               (attackBoard<us, Pawn>(sq) & board->getPieces<them, Pawn>()) |
               (attackBoard<King>(sq) & board->getPieces<them, King>());
    }

    namespace MoveFactory {
        template<FilterType FT>
        int generateMoves(Board*, Move*);
    }
}


#endif //CHARON_MOVEMAKE_H
