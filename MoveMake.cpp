//
// Created by evcmo on 7/4/2021.
//

#include "MoveMake.h"

namespace Charon {

    namespace {

        /**
         * A function to determine whether all squares represented
         * by high bits in the given bitboard are safe to travel to.
         *
         * @tparam A    the alliance to consider
         * @param board the current game board
         * @param d     the bitboard to check for safe squares
         * @return      whether or not all squares represented
         *              by high bits in the given bitboard are
         *              safe to travel to
         */
        template <Alliance A> [[nodiscard]]
        inline bool safeSquares(Board* const board,
                                const uint64_t destinations) {
            static_assert(A == White || A == Black);
            for (uint64_t d = destinations; d; d &= d - 1)
                if (attacksOn<A, NullPT>(board, bitScanFwd(d)))
                    return false;
            return true;
        }

        /**
         * A function to generate this Player's pawn moves.
         *
         * ToDo: add promotion capability.
         *
         * @tparam A        the alliance
         * @tparam FT       the filter type
         * @param board     the board to use
         * @param checkMask the check mask to use in the
         *                  case of check or double check
         * @param kingGuard a bitboard representing the
         *                  pieces that block sliding
         *                  attacks on the king
         *                  for the given alliance
         * @param moves     a pointer to a list to populate
         *                  with moves
         * @return          a pointer to the next empty
         *                  index in the array that holds
         *                  the moves list
         */
        template <Alliance A, FilterType FT>
        Move* makePawnMoves(Board* const board,
                            const uint64_t checkMask,
                            const uint64_t kingGuard,
                            const int kingSquare,
                            Move* moves) {
            static_assert(A == White || A == Black);
            static_assert(FT >= Aggressive && FT <= All);

            constexpr const Alliance us = A, them = ~us;

            // Initialize constants.
            const uint64_t enemies      = board->getPieces<them>() & checkMask,
                           emptySquares = ~board->getAllPieces(),
                           pawns        = board->getPieces<us, Pawn>(),
                           king         = board->getPieces<us, King>(),
                           freePawns    = pawns & ~kingGuard,
                           pinnedPawns  = pawns & kingGuard;

            // Determine defaults.
            constexpr const Defaults* const x =
                    us == White? &WhiteDefaults: &BlackDefaults;

            // If generating passive moves or all moves.
            if (FT != Aggressive) {
                // Generate single and double pushes for free pawns.
                // All legal, passive targets one square ahead.
                uint64_t p1 = shift<x->up>(freePawns) & emptySquares;

                // All legal, passive targets two squares ahead.
                uint64_t p2 = shift<x->up>(p1 & x->pawnJumpSquares)
                    & emptySquares;

                // Intersect the pushes with the current checkMask.
                p1 &= checkMask;
                p2 &= checkMask;

                // Make moves from passive one-square targets.
                for (int d; p1; p1 &= p1 - 1) {
                    d = bitScanFwd(p1);
                    *moves++ = Move::make((d + x->down), d);
                }

                // Make moves from passive two-square targets.
                for (int d; p2; p2 &= p2 - 1) {
                    d = bitScanFwd(p2);
                    *moves++ =
                        Move::make<PawnJump>((d + x->down + x->down), d);
                }
            }

            // If generating all moves or attack moves, continue,
            // otherwise return.
            if (FT != Passive) {
                // Generate left and right attack moves for free pawns.
                // All legal aggressive targets one square ahead and
                // to the right.
                uint64_t ar =
                    shift<x->upRight>(freePawns & x->notRightCol)
                        & enemies;
                // All legal aggressive targets one square ahead and
                // to the left.
                uint64_t al =
                    shift<x->upLeft>(freePawns & x->notLeftCol)
                        & enemies;

                // Make moves from aggressive right targets.
                for (int d; ar; ar &= ar - 1) {
                    d = bitScanFwd(ar);
                    *moves++ = Move::make(d + x->downLeft, d);
                }

                // Make moves from aggressive left targets.
                for (int d; al; al &= al - 1) {
                    d = bitScanFwd(al);
                    *moves++ = Move::make(d + x->downRight, d);
                }
            }

            // Generate single and double pushes for pinned pawns,
            // if any.
            // Generate left and right attack moves for pinned pawns,
            // if any.
            if (pinnedPawns) {
                if(FT != Aggressive) {
                    // All pseudo-legal, passive targets one square ahead.
                    uint64_t p1 = shift<x->up>(pinnedPawns) & emptySquares;

                    // All pseudo-legal, passive targets two squares ahead.
                    uint64_t p2 = shift<x->up>(p1 & x->pawnJumpSquares)
                                  & emptySquares;

                    // Intersect the pushes with the current checkMask.
                    p1 &= checkMask;
                    p2 &= checkMask;

                    // Make legal moves from passive one-square
                    // pseudo-legal targets.
                    // These moves must have a destination on
                    // the pinning ray.
                    for (int d, o; p1; p1 &= p1 - 1) {
                        d = bitScanFwd(p1);
                        o = d + x->down;
                        if (rayBoard(kingSquare, o) & p1 &
                            (uint64_t) -(int64_t) p1)
                            *moves++ = Move::make(o, d);
                    }

                    // Make legal moves from passive two-square
                    // pseudo-legal targets.
                    // These moves must have a destination on
                    // the pinning ray.
                    for (int d, o; p2; p2 &= p2 - 1) {
                        d = bitScanFwd(p2);
                        o = d + x->down + x->down;
                        if (rayBoard(kingSquare, o) & p2 &
                            (uint64_t) -(int64_t) p2)
                            *moves++ = Move::make<PawnJump>(o, d);
                    }
                }

                if(FT != Passive) {
                    // All pseudo-legal aggressive targets one square ahead
                    // and to the right.
                    uint64_t ar =
                        shift<x->upRight>(pinnedPawns & x->notRightCol)
                            & enemies;

                    // All pseudo-legal aggressive targets one square ahead
                    // and to the left.
                    uint64_t al =
                        shift<x->upLeft>(pinnedPawns & x->notLeftCol)
                            & enemies;

                    // Make legal moves from pseudo-legal aggressive
                    // right targets. Only consider destinations that
                    // lie on the pinning ray.
                    for (int d, o; ar; ar &= ar - 1) {
                        d = bitScanFwd(ar);
                        o = d + x->downLeft;
                        if (rayBoard(kingSquare, o) & ar &
                           (uint64_t) -(int64_t) ar)
                            *moves++ = Move::make(o, d);
                    }

                    // Make legal moves from pseudo-legal aggressive
                    // left targets. Only consider destinations that
                    // lie on the pinning ray.
                    for (int d, o; al; al &= al - 1) {
                        d = bitScanFwd(al);
                        o = d + x->downRight;
                        if (rayBoard(kingSquare, o) & al &
                           (uint64_t) -(int64_t) al)
                            *moves++ = Move::make(o, d);
                    }
                }
            }

            // If the filter type is not passive, continue.
            if(FT == Passive) return moves;

            // Find the en passant square, if any.
            const int enPassantSquare = board->getEpSquare();

            // If the en passant square is set, continue.
            if (enPassantSquare == NullSQ) return moves;

            // The en passant pawn square board.
            const uint64_t eppBoard = SquareToBitBoard[enPassantSquare];

            // If we are in single check and the destination square
            // doesn't block... Don't generate an en passant move.
            if (!((eppBoard + x->up) & checkMask)) return moves;

            // Check for passing pawns.
            const uint64_t rightPass =
                    shift<x->right>(eppBoard & x->notRightCol) & freePawns,
                           leftPass  =
                    shift<x->left >(eppBoard & x->notLeftCol ) & freePawns;

            // If there is a passing pawn, generate legal
            // en passant moves.
            if (!(rightPass | leftPass)) return moves;

            // If the king is on the en passant rank then
            // an en passant discovered check is possible.
            if (king & x->enPassantRank) {
                // Find the snipers on the en passant rank.
                const uint64_t snipers =
                        (board->getPieces<them, Queen>() |
                         board->getPieces<them, Rook>()) &
                         x->enPassantRank;

                // Check to see if the en passant pawn
                // (and attacking pawn) are between any of
                // the snipers and the king square.
                // If so, there is an en passant discovered
                // check on the board, and an en passant move
                // cannot be generated.
                for (uint64_t s = snipers; s; s &= s - 1)
                    if (eppBoard & pathBoard(bitScanFwd(s), kingSquare))
                        return moves;
            }

            // If the right pass square holds a pawn, add an en
            // passant move for that pawn.
            if (rightPass) {
                const uint64_t o = bitScanFwd(rightPass);
                *moves++ =
                    Move::make<EnPassant>(o, (o + x->upLeft));
            }

            // If the left pass square holds a pawn, add an en
            // passant move for that pawn.
            if (leftPass) {
                const uint64_t o = bitScanFwd(leftPass);
                *moves++ =
                    Move::make<EnPassant>(o, (o + x->upRight));
            }

            return moves;
        }

        /**
         * A function to generate the moves for a given piece
         * type.
         *
         * @tparam A        the alliance to consider
         * @tparam PT       the piece type to consider
         * @param board     the current game board
         * @param kingGuard the king guard for the given
         *                  alliance
         * @param filter    the filter mask to use
         * @param moves     a pointer to a list to
         *                  populate with moves
         * @return          a pointer to the next empty
         *                  index in the array that holds
         *                  the moves list
         */
        template<Alliance A, PieceType PT>
        Move* makeMoves(Board* const board,
                               const uint64_t kingGuard,
                               const uint64_t filter,
                               const int kingSquare,
                               Move* moves) {
            static_assert(A == White || A == Black);
            static_assert(PT >= Rook && PT <= Queen);

            constexpr const Alliance us = A;

            // Initialize constants.
            const uint64_t pieceBoard = board->getPieces<us, PT>(),
                           freePieces = pieceBoard & ~kingGuard,
                           allPieces  = board->getAllPieces();

            // Calculate moves for free pieces.
            // Traverse the free piece bit board.
            for (uint64_t n = freePieces; n; n &= n - 1) {

                // Find an origin square.
                const int origin = bitScanFwd(n);

                // Look up the attack board using the origin
                // square and intersect with the filter.
                uint64_t ab = attackBoard<PT>(allPieces, origin)
                    & filter;

                // Traverse through the legal
                // move board and add all legal moves.
                for (; ab; ab &= ab - 1)
                    *moves++ = Move::make(origin, bitScanFwd(ab));
            }

            // Knight pinned pieces are trapped. They
            // cannot move along the pinning ray.
            if (PT == Knight) return moves;

            // All pieces pinned between the king and an
            // attacker.
            const uint64_t pinnedPieces = pieceBoard & kingGuard;

            // If there are pinned pieces, generate their legal
            // moves.
            if (pinnedPieces) {
                // Calculate moves for pinned pieces.
                // Traverse the pinned piece bitboard.
                uint64_t n = pinnedPieces;
                do {

                    // Find an origin square.
                    const int origin = bitScanFwd(n);

                    // Lookup the attack board and intersect with
                    // the filter and the pinning ray.
                    uint64_t ab =
                        attackBoard<PT>(allPieces, origin)
                            & filter & rayBoard(kingSquare, origin);

                    // Traverse through the legal
                    // move board and add all legal moves.
                    for (; ab; ab &= ab - 1)
                        *moves++ = Move::make(origin, bitScanFwd(ab));

                    n &= n - 1;
                } while (n);
            }

            return moves;
        }

        /**
         * A function to generate moves for every piece type.
         *
         * @tparam A    the alliance to consider
         * @tparam FT   the filter type
         * @param board the current game board
         * @param moves a pointer to the list to populate
         */
        template <Alliance A, FilterType FT>
        int makeMoves(Board* const board, Move* moves) {
            static_assert(A == White || A == Black);
            static_assert(FT >= Aggressive && FT <= All);

            const Move* const initialMove = moves;
            constexpr const Alliance us = A, them = ~us;

            // Initialize constants.
            const uint64_t allPieces     = board->getAllPieces(),
                           ourPieces     = board->getPieces<us>(),
                           theirPieces   = board->getPieces<them>(),
                           partialFilter = FT == All?     ~ourPieces :
                                           FT == Passive? ~allPieces :
                                                          theirPieces,
                           king          = board->getPieces<us, King>();

            // Get the board defaults for our alliance.
            constexpr const Defaults* const x =
                    us == White? &WhiteDefaults: &BlackDefaults;

            // Find the king square.
            const int ksq = bitScanFwd(king);

            // Find all pieces that attack our king.
            const uint64_t checkBoard = attacksOn<us, King>(board, ksq);

            // Calculate the check type for our king.
            const CheckType checkType = calculateCheck(checkBoard);

            // If our king is in double check, then only king moves
            // should be considered.
            if (checkType != DoubleCheck) {
                uint64_t blockers = 0;
                const uint64_t theirQueens =
                        board->getPieces<them, Queen>();

                // Find the sniper pieces.
                const uint64_t snipers =
                        (attackBoard<Rook>(0, ksq)          &
                        (board->getPieces<them, Rook>()   | theirQueens)) |
                        (attackBoard<Bishop>(0, ksq)        &
                        (board->getPieces<them, Bishop>() | theirQueens));

                // Iterate through the snipers and draw paths to the king,
                // using these paths as an x-ray to find the blockers.
                for (uint64_t s = snipers; s; s &= s - 1) {
                    const int      ssq     = bitScanFwd(s);
                    const uint64_t blocker = pathBoard(ssq, ksq) &
                            allPieces & ~(s & -s) & ~king;
                    if(blocker && !(blocker & (blocker - 1))) blockers |= blocker;
                }

                // Determine which friendly pieces block sliding attacks on our
                // king.
                // If our king is in single check, determine the path between the
                // king and his attacker.
                const uint64_t kingGuard = ourPieces & blockers,
                               checkMask = (checkType == Check ? (pathBoard(
                                      ksq, bitScanFwd(checkBoard)
                               )) | checkBoard: FullBoard),
                // A filter to limit all pieces to evasion
                // moves only in the event of single check
                // while simultaneously allowing the client
                // to limit generation to attack, passive,
                // or all move types.
                               fullFilter = partialFilter & checkMask;

                // Make non-king moves.
                moves = makePawnMoves<us, FT>(board, checkMask,  kingGuard, ksq, moves);
                moves = makeMoves<us, Knight>(board, kingGuard, fullFilter, ksq, moves);
                moves = makeMoves<us, Bishop>(board, kingGuard, fullFilter, ksq, moves);
                moves = makeMoves<us,   Rook>(board, kingGuard, fullFilter, ksq, moves);
                moves = makeMoves<us,  Queen>(board, kingGuard, fullFilter, ksq, moves);
            }

            // Generate normal king moves.

            uint64_t d = SquareToKingAttacks[ksq];
            for (d &= partialFilter; d; d &= d - 1) {
                const int dest = bitScanFwd(d);
                if (!attacksOn<us, King>(board, dest))
                    *moves++ = Move::make(ksq, dest);
            }

            // If the filter type is aggressive, then castling moves
            // are irrelevant. If we don't have castling rights or
            // if we are in check, then castling moves are illegal.
            if (FT == Aggressive || checkType != None)
                return (moves - initialMove);

            // Generate king-side castle.
            if(!(x->kingSideMask & allPieces)
               && board->hasCastlingRights<us, KingSide>()
               && safeSquares<us>(board, x->kingSideMask))
                *moves++ = Move::make<Castling>(
                        ksq, x->kingSideDestination
                );

            // Generate queen-side castle.
            if(!(x->queenSideMask & allPieces)
               && board->hasCastlingRights<us, QueenSide>()
               && safeSquares<us>(board, x->queenSideMask))
                *moves++ = Move::make<Castling>(
                        ksq, x->queenSideDestination
                );

            return (moves - initialMove);
        }
    } // namespace (anon)

    namespace MoveFactory {

        /**
         * <summary>
         *  <p><br/>
         * A function to generate moves for the given
         * board according to the given filter type,
         * by populating the given list of MoveWrap
         * structures.
         *  </p>
         *  <p>
         *  The filter type must be one of the following:
         *   <ul>
         *    <li>
         *     <b><i>All</i></b>
         *     <p>
         *  This filter will cause the function to
         *  return a list of all moves, both passive
         *  and aggressive.
         *     </p>
         *    </li>
         *    <li>
         *     <b><i>Passive</i></b>
         *     <p>
         *  This filter will cause the function to
         *  return a list of all non-capture moves.
         *     </p>
         *    </li>
         *    <li>
         *     <b><i>Aggressive</i></b>
         *     <p>
         *  This filter will cause the function to
         *  return a list of all capture moves.
         *     </p>
         *    </li>
         *   </ul>
         *  </p>
         * </summary>
         *
         * @tparam FT the filter type.
         * @param board the current game board
         * @param moves an empty list of MoveWrap
         * structures to hold legal moves.
         */
        template<FilterType FT>
        inline int generateMoves(Board *const board, Move* const moves) {
            static_assert(FT >= Aggressive && FT <= All);
            return board->currentPlayer() == White ?
                   makeMoves<White, FT>(board, moves) :
                   makeMoves<Black, FT>(board, moves);
        }

        // Explicit instantiations.
        template int generateMoves<Aggressive>(Board*, Move*);
        template int generateMoves<Passive>(Board*, Move*);
        template int generateMoves<All>(Board*, Move*);
    }
} // namespace Charon