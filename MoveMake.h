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
        // Define players.
        const Player* const ourPlayer   = board->getPlayer<us>();
        const Player* const theirPlayer = board->getPlayer<them>();
        // Get their queen.

        const uint64_t theirQueens = theirPlayer->getPieces<Queen>(),
                target      = ourPlayer->getPieces<PT>(),
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
                (theirPlayer->getPieces<Rook>()   | theirQueens)) |
               (diagonalAttackMask &
                (theirPlayer->getPieces<Bishop>() | theirQueens)) |
               (attackBoard<Knight>(sq) & theirPlayer->getPieces<Knight>()) |
               (attackBoard<us, Pawn>(sq) & theirPlayer->getPieces<Pawn>())
                          :
               (onAxisAttackMask &
                (theirPlayer->getPieces<Rook>()   | theirQueens)) |
               (diagonalAttackMask &
                (theirPlayer->getPieces<Bishop>() | theirQueens)) |
               (attackBoard<Knight>(sq) & theirPlayer->getPieces<Knight>()) |
               (attackBoard<us, Pawn>(sq) & theirPlayer->getPieces<Pawn>()) |
               (attackBoard<King>(sq) & theirPlayer->getPieces<King>());
    }

    namespace MoveFactory {
        template<FilterType FT>
        int generateMoves(Board *, MoveWrap *);
    }
}


#endif //CHARON_MOVEMAKE_H
