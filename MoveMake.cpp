//
// Created by evcmo on 7/4/2021.
//

#include "MoveMake.h"

namespace Charon {

    namespace {

        /**
         * A function to calculate attacks on the fly.
         *
         * @tparam A    the alliance of the piece
         *              on the square under attack
         * @tparam PT   the piece type
         * @param board the current game board
         * @param sq    the square under attack
         */
        template<Alliance A, PieceType PT> [[nodiscard]]
        uint64_t attacksOn(Board* const board, const int sq) {
            static_assert(A == White || A == Black);
            static_assert(PT >= Pawn && PT <= NullPT);

            // Determine alliances.
            constexpr const Alliance us = A, them = ~us;
            // Define players.
            const Player* const ourPlayer = board->getPlayer<us>();
            const Player* const theirPlayer = board->getPlayer<them>();
            // Get their queen.

            const uint64_t theirQueens = theirPlayer->getPieces<Queen>(),
                           target = ourPlayer->getPieces<PT>(),
            // Board minus king
                           allPieces   = board->getAllPieces() & ~target;
            // Get the attack board for horizontal and vertical attacks
            // on the given square.
            const uint64_t onAxisAttackMask =
                    attackBoard<Rook>(allPieces, sq);
            // Get the attack board for diagonal attacks on the given
            // square.
            const uint64_t diagonalAttackMask =
                    attackBoard<Bishop>(allPieces, sq);

            // Calculate and return a bitboard representing all attackers.
            return PT == King?
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
        MoveWrap* makePawnMoves(Board* const board,
                                const uint64_t checkMask,
                                const uint64_t kingGuard,
                                MoveWrap* moves) {
            static_assert(A == White || A == Black);
            static_assert(FT >= Aggressive && FT <= All);

            // Determine alliances.
            constexpr const Alliance us = A, them = ~us;
            // Define players.
            Player* const ourPlayer   = board->getPlayer<us>()  ;
            Player* const theirPlayer = board->getPlayer<them>();
            // The intersection of All pieces of belonging to the
            // opponent and the specified check mask.
            const uint64_t enemies =
                          theirPlayer->getAllPieces() & checkMask,
            // All pieces of both alliances represented by 1's.
                           emptySquares =
                          ~board->getAllPieces() & checkMask;
            // The current enPassant square - guaranteed to belong
            // to the opponent.
            const int enPassantSquare = board->getEpSquare();
            const uint64_t pawns      = ourPlayer->getPieces<Pawn>(),
                           king       = ourPlayer->getPieces<King>(),
            // All pawns that are not in the king guard.
                           freePawns   = pawns & ~kingGuard,
            // All pawns that are in the king guard (pinned to a ray).
                           pinnedPawns = pawns & kingGuard;

            // Determine defaults.
            constexpr const Defaults* const x = getDefaults<us>();

            // If generating passive moves or all moves.
            if (FT != Aggressive) {
                // Generate single and double pushes for free pawns.
                {
                    // All legal, passive targets one square ahead.
                    uint64_t p1 = shift<x->up>(freePawns) & emptySquares;

                    // All legal, passive targets two squares ahead.
                    uint64_t p2 = freePawns & x->pawnStart;
                        p2 = p2? shift<x->up>(shift<x->up>(p2) & p1)
                            & emptySquares : p2;

                    // Make moves from passive one-square targets.
                    for (int dest; p1; p1 &= p1 - 1) {
                        dest = bitScanFwd(p1);
                        *moves++ = Move::make(
                                (dest + x->down), dest
                        );
                    }

                    // Make moves from passive two-square targets.
                    for (int dest; p2; p2 &= p2 - 1) {
                        dest = bitScanFwd(p2);
                        *moves++ = Move::make<PawnJump>(
                                ((dest + x->down) + x->down), dest
                        );
                    }
                }

                // Generate single and double pushes for pinned pawns,
                // if any.
                if (pinnedPawns) {
                    {
                        // All pseudo-legal, passive targets one square ahead.
                        uint64_t p1 = shift<x->up>(pinnedPawns) & emptySquares;

                        // All pseudo-legal, passive targets two squares ahead.
                        uint64_t p2 = pinnedPawns & x->pawnStart;
                            p2 = p2? shift<x->up>(shift<x->up>(p2) & p1)
                                & emptySquares : p2;

                        const int ksq = bitScanFwd(king);

                        // Make legal moves from passive one-square
                        // pseudo-legal targets.
                        // These moves must have a destination on
                        // the pinning ray.
                        for (int dest, ori; p1; p1 &= p1 - 1) {
                            dest = bitScanFwd(p1);
                            ori = dest + x->down;
                            if (
                                rayBoard(ksq, ori) & p1 &
                                (uint64_t) -(int64_t) p1)
                                *moves++ = Move::make(
                                        ori, dest
                                );
                        }

                        // Make legal moves from passive two-square
                        // pseudo-legal targets.
                        // These moves must have a destination on
                        // the pinning ray.
                        for (int dest, ori; p2; p2 &= p2 - 1) {
                            dest = bitScanFwd(p2);
                            ori = dest + x->down + x->down;
                            if (
                                rayBoard(ksq, ori) & p2 &
                                (uint64_t) -(int64_t) p2)
                                *moves++ = Move::make<PawnJump>(
                                        ori, dest
                                );
                        }
                    }
                }
            }

            // If generating all moves or attack moves, continue,
            // otherwise return.
            if (FT == Passive) return moves;
            { // Generate left and right attack moves for free pawns.
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
                for (int dest; ar; ar &= ar - 1) {
                    dest = bitScanFwd(ar);
                    *moves++ = Move::make(
                             dest + x->downLeft, dest
                    );
                }

                // Make moves from aggressive left targets.
                for (int dest; al; al &= al - 1) {
                    dest = bitScanFwd(al);
                    *moves++ = Move::make(
                             dest + x->downRight, dest
                    );
                }
            }

            // Generate left and right attack moves for pinned pawns,
            // if any.
            if (pinnedPawns) {
                {
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

                    const int ksq = bitScanFwd(king);

                    // Make legal moves from pseudo-legal aggressive
                    // right targets. Only consider destinations that
                    // lie on the pinning ray.
                    for (int dest, ori; ar; ar &= ar - 1) {
                        dest = bitScanFwd(ar);
                        ori = dest + x->downLeft;
                        if (
                            rayBoard(ksq, ori) & ar &
                            (uint64_t) -(int64_t) ar)
                            *moves++ = Move::make(
                                    ori, dest
                            );
                    }

                    // Make legal moves from pseudo-legal aggressive
                    // left targets. Only consider destinations that
                    // lie on the pinning ray.
                    for (int dest, ori; al; al &= al - 1) {
                        dest = bitScanFwd(al);
                        ori = dest + x->downRight;
                        if (
                            rayBoard(ksq, ori) & al &
                            (uint64_t) -(int64_t) al)
                            *moves++ = Move::make(
                                    ori, dest
                            );
                    }
                }
            }

            // If the en passant square is set, check for
            // passing pawns
            if (enPassantSquare == NullSQ) return moves;

            // The en passant pawn square.
            const uint64_t eppsq = SquareToBitBoard[enPassantSquare];

            // If we are in single check and the destination square
            // doesn't block... Don't generate an en passant move.
            if(!(shift<x->down>(eppsq) & checkMask)) return moves;

            // Check for passing pawns
            const uint64_t rightPass = shift<x->right>(eppsq) & freePawns,
                           leftPass  = shift<x->left>(eppsq)  & freePawns;

            // If there is a passing pawn, generate legal
            // en passant moves.
            if(!(rightPass | leftPass)) return moves;

            // If the king is on the en passant rank then
            // an en passant discovered check is possible.
            if(king & x->enPassantRank) {

                // Find the snipers on the en passant rank.
                const uint64_t snipers =
                        (theirPlayer->getPieces<Queen>() |
                         theirPlayer->getPieces<Rook>()) &
                        x->enPassantRank;
                // Find the king square.
                const int ksq          = bitScanFwd(king);

                // Check to see if the en passant pawn
                // (and attacking pawn) are between any of
                // the snipers and the king square.
                // If so, there is an en passant discovered
                // check on the board, and an en passant move
                // cannot be generated.
                for (uint64_t s = snipers; s; s &= s - 1)
                    if (eppsq & pathBoard(bitScanFwd(s), ksq))
                        return moves;
            }

            // If the right pass square holds a pawn, add an en
            // passant move for that pawn.
            if (rightPass) {
                const uint64_t origin = bitScanFwd(rightPass);
                *moves++ = Move::make<EnPassant>(
                        origin, (origin + x->upLeft)
                );
            }

            // If the left pass square holds a pawn, add an en
            // passant move for that pawn.
            if (leftPass) {
                const uint64_t origin = bitScanFwd(leftPass);
                *moves++ = Move::make<EnPassant>(
                        origin, (origin + x->upRight)
                );
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
        inline MoveWrap* makeMoves(Board* const board,
                                   const uint64_t kingGuard,
                                   const uint64_t filter,
                                   MoveWrap* moves) {
            static_assert(A == White || A == Black);
            static_assert(PT >= Rook && PT <= Queen);

            // Get our player.
            Player* const ourPlayer   = board->getPlayer<A>();
            // Get the bitboard representing the given piece type
            // for our player.
            const uint64_t pieceBoard = ourPlayer->getPieces<PT>(),
            // All pieces that are free to move in any
            // legal direction.
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
                    *moves++ = Move::make(
                            origin, bitScanFwd(ab)
                    );
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
                // Find the king square for this player.
                const int ksq =
                    bitScanFwd(ourPlayer->getPieces<King>());

                uint64_t n = pinnedPieces;
                do {

                    // Find an origin square.
                    const int origin = bitScanFwd(n);

                    // Lookup the attack board and intersect with
                    // the filter and the pinning ray.
                    uint64_t ab =
                        attackBoard<PT>(allPieces, origin)
                            & filter & rayBoard(ksq, origin);

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
         * A function to generate all king moves, including
         * castling moves.
         *
         * @tparam A     the alliance to consider
         * @tparam FT    the filter type
         * @param board  the current game board
         * @param ct     the check type
         * @param filter
         * @param moves
         */
        template <Alliance A, FilterType FT>
        void makeKingMoves(Board* const board,
                           const CheckType ct,
                           const uint64_t filter,
                           MoveWrap* moves) {
            static_assert(A == White || A == Black);
            static_assert(FT >= Aggressive && FT <= All);

            // Define our player.
            Player* const ourPlayer = board->getPlayer<A>();
            // Find our king.
            const uint64_t king = ourPlayer->getPieces<King>();
            // Find our king's square.
            const int ksq = bitScanFwd(king);
            // Get the board defaults for our alliance.
            constexpr const Defaults* const x = getDefaults<A>();

            { // Generate normal king moves.
                uint64_t d = SquareToKingAttacks[ksq];
                for (d &= filter; d; d &= d - 1) {
                    const int dest = bitScanFwd(d);
                    if (!attacksOn<A, King>(board, dest))
                        *moves++ = Move::make(
                                ksq, dest
                        );
                }
            }

            // If the filter type is aggressive, then castling moves
            // are irrelevant. If we don't have castling rights or
            // if we are in check, then castling moves are illegal.
            if (FT == Aggressive || ct != None) return;

            if(ourPlayer->hasCastlingRights<KingSide>()) {
                // Generate king-side castle.
                uint64_t d = x->kingSideMask & ~board->getAllPieces();
                if(d == x->kingSideMask && safeSquares<A>(board, d))
                    *moves++ = Move::make<Castling>(
                            ksq, x->kingSideDestination
                    );
            }

            if(ourPlayer->hasCastlingRights<QueenSide>()) {
                // Generate queen-side castle.
                uint64_t d = x->queenSideMask & ~board->getAllPieces();
                if(d == x->queenSideMask && safeSquares<A>(board, d))
                    *moves = Move::make<Castling>(
                            ksq, x->queenSideDestination
                    );
            }
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
        void makeMoves(Board* const board, MoveWrap* moves) {
            static_assert(A == White || A == Black);
            static_assert(FT >= Aggressive && FT <= All);

            // Determine alliances.
            constexpr const Alliance us    = A, them = ~us;
            // get our player and their player.
            Player* const ourPlayer   = board->getPlayer<us>();
            Player* const theirPlayer = board->getPlayer<them>();
            // get all pieces on the board.
            const uint64_t allPieces   = board->getAllPieces(),
                           ourPieces   = ourPlayer->getAllPieces(),
                           theirPieces = theirPlayer->getAllPieces(),
            // Create the partial filter according to the filter type.
            partialFilter = FT == All? ~ourPieces :
                            FT == Passive? ~allPieces :
                            theirPieces,
            // Get our king board.
            king       = ourPlayer->getPieces<King>(),
            // Find all attacks on our king.
            checkBoard = attacksOn<us, King>(board, bitScanFwd(king));
            // Calculate the check type for our king.
            const CheckType checkType = calculateCheck(checkBoard);

            // If our king is in double check, then only king moves
            // should be considered.
            if (checkType != DoubleCheck) {

                // Find the king square.
                const int ksq     = bitScanFwd(king);
                uint64_t blockers = 0;

                // Find the sniper pieces.
                const uint64_t snipers =
                        (attackBoard<Rook>(0, ksq)          &
                         (theirPlayer->getPieces<Rook>()    |
                          theirPlayer->getPieces<Queen>())) |
                        (attackBoard<Bishop>(0, ksq)        &
                         (theirPlayer->getPieces<Bishop>()  |
                          theirPlayer->getPieces<Queen>()));

                // Iterate through the snipers and draw paths to the king,
                // using these paths as an x-ray to find the blockers.
                for(uint64_t s = snipers; s; s &= s - 1) {
                    const int      ssq     = bitScanFwd(s);
                    const uint64_t blocker = pathBoard(ssq, ksq) & allPieces;
                    if(blocker && !(blocker & (blocker - 1))) blockers |= blocker;
                }

                // Determine which friendly pieces block sliding attacks on our
                // king.
                const uint64_t kingGuard = ourPieces & blockers,
                // If our king is in single check, determine the path between the
                // king and his attacker.
                checkMask = checkType == Check ? pathBoard(
                        bitScanFwd(king), bitScanFwd(checkBoard)
                ) : FullBoard,
                // A filter to limit all pieces to evasion
                // moves only in the event of single check
                // while simultaneously allowing the client
                // to limit generation to attack, passive,
                // or all move types.
                fullFilter = partialFilter & checkMask;

                // Make non-king moves.
                moves = makePawnMoves<us, FT>(board, checkMask,  kingGuard, moves);
                moves = makeMoves<us, Knight>(board, kingGuard, fullFilter, moves);
                moves = makeMoves<us, Bishop>(board, kingGuard, fullFilter, moves);
                moves = makeMoves<us,   Rook>(board, kingGuard, fullFilter, moves);
                moves = makeMoves<us,  Queen>(board, kingGuard, fullFilter, moves);
            }
            // make king moves.
            makeKingMoves<us, FT>(board, checkType, partialFilter, moves);
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
        void generateMoves(Board *const board, MoveWrap *const moves) {
            static_assert(FT >= Aggressive && FT <= All);
            return board->currentPlayer() == White ?
                   makeMoves<White, FT>(board, moves) :
                   makeMoves<Black, FT>(board, moves);
        }

        // Explicit instantiations.
        template void generateMoves<Aggressive>(Board *, MoveWrap *);
        template void generateMoves<Passive>(Board *, MoveWrap *);
        template void generateMoves<All>(Board *, MoveWrap *);
    }
} // namespace Charon