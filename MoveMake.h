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
     * A function to find attackers on the fly.
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
                       target      = PT == NullPT? 0: board->getPieces<us, PT>(),
                       allPieces   = board->getAllPieces() & ~target;

        // Calculate and return a bitboard representing all attackers.
        return (attackBoard<Rook>(allPieces, sq)   &
               (board->getPieces<them, Rook>()   | theirQueens)) |
               (attackBoard<Bishop>(allPieces, sq) &
               (board->getPieces<them, Bishop>() | theirQueens)) |
               (SquareToKnightAttacks[sq]   & board->getPieces<them, Knight>()) |
               (SquareToPawnAttacks[us][sq] & board->getPieces<them, Pawn>())   |
               (SquareToKingAttacks[sq]     & board->getPieces<them, King>());
    }

    /**
     * A function to calculate check for our king given a
     * bitboard representing the pieces that attack its
     * square.
     *
     * @param checkBoard a bitboard of all pieces that attack
     * our king
     * @return whether or not our king is in check
     */
    constexpr CheckType
    calculateCheck(uint64_t checkBoard) {
        return !checkBoard ? None:
               (checkBoard & (checkBoard - 1)) ?
               DoubleCheck: Check;
    }

    namespace MoveFactory {

        /**
         * <summary>
         *  <p><br/>
         * A function to generate moves for the given
         * board according to the given filter type,
         * by populating the given list.
         *  </p>
         *  <p>
         *  The filter type must be one of the following:
         *   <ul>
         *    <li>
         *     <b><i>All</i></b>
         *     <p>
         *  This filter will cause the function to
         *  return a list of all legal moves, both
         *  passive and aggressive.
         *     </p>
         *    </li>
         *    <li>
         *     <b><i>Passive</i></b>
         *     <p>
         *  This filter will cause the function to
         *  return a list of all legal non-capture
         *  moves.
         *     </p>
         *    </li>
         *    <li>
         *     <b><i>Aggressive</i></b>
         *     <p>
         *  This filter will cause the function to
         *  return a list of all legal capture moves.
         *     </p>
         *    </li>
         *   </ul>
         *  </p>
         * </summary>
         *
         * @tparam FT the filter type
         * @param board the current game board
         * @param moves an empty list of moves
         */
        template<FilterType FT>
        int generateMoves(Board*, Move*);
    }
}


#endif //CHARON_MOVEMAKE_H
