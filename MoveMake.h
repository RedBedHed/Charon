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

        constexpr const Alliance us = A, them = ~us;

        // Initialize constants.
        const uint64_t theirQueens = board->getPieces<them, Queen>(),
                       target      = board->getPieces<us, PT>(),
                       allPieces   = board->getAllPieces() & ~target;

        // Calculate and return a bitboard representing all attackers.
        return PT == King ?
               (attackBoard<Rook>(allPieces, sq)   &
                (board->getPieces<them, Rook>()   | theirQueens)) |
               (attackBoard<Bishop>(allPieces, sq) &
                (board->getPieces<them, Bishop>() | theirQueens)) |
               (SquareToKnightAttacks[sq]   & board->getPieces<them, Knight>()) |
               (SquareToPawnAttacks[us][sq] & board->getPieces<them, Pawn>())
                          :
               (attackBoard<Rook>(allPieces, sq)   &
                (board->getPieces<them, Rook>()   | theirQueens)) |
               (attackBoard<Bishop>(allPieces, sq) &
                (board->getPieces<them, Bishop>() | theirQueens)) |
               (SquareToKnightAttacks[sq]   & board->getPieces<them, Knight>()) |
               (SquareToPawnAttacks[us][sq] & board->getPieces<them, Pawn>())   |
               (SquareToKingAttacks[sq]     & board->getPieces<them, King>());
    }

    constexpr CheckType
    calculateCheck(uint64_t checkBoard) {
        return !checkBoard ? None:
               (checkBoard & (checkBoard - 1)) ?
               DoubleCheck: Check;
    }

    namespace MoveFactory {
        template<FilterType FT>
        int generateMoves(Board*, Move*);
    }
}


#endif //CHARON_MOVEMAKE_H
