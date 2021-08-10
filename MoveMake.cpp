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
         * A function to make promotions and under-promotions.
         *
         * @param moves a pointer to a list to populate
         *              with moves
         * @param o     the origin square
         * @param d     the destination square
         */
        inline Move*
        makePromotions(Move* moves, const int o, const int d) {
            *moves++ = Move::makePromotion<Rook  >(o, d);
            *moves++ = Move::makePromotion<Knight>(o, d);
            *moves++ = Move::makePromotion<Bishop>(o, d);
            *moves++ = Move::makePromotion<Queen >(o, d);
            return moves;
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

            // Determine defaults.
            constexpr const Defaults* const x = defaults<us>();

            // Initialize constants.
            const uint64_t enemies         = board->getPieces<them>() & checkMask,
                           emptySquares    = ~board->getAllPieces(),
                           pawns           = board->getPieces<us, Pawn>(),
                           king            = board->getPieces<us, King>(),
                           freePawns       = pawns & ~kingGuard,
                           pinnedPawns     = pawns & kingGuard,
                           freeLowPawns    = freePawns & ~x->prePromotionMask,
                           freeHighPawns   = freePawns & x->prePromotionMask,
                           pinnedLowPawns  = pinnedPawns & ~x->prePromotionMask,
                           pinnedHighPawns = pinnedPawns & x->prePromotionMask;

            // If generating passive moves or all moves.
            if (FT != Aggressive) {
                // Generate single and double pushes for free low pawns.
                // All pseudo-legal, passive targets one square ahead.
                uint64_t p1 = shift<x->up>(freeLowPawns) & emptySquares;

                // All pseudo-legal, passive targets two squares ahead.
                uint64_t p2 = shift<x->up>(p1 & x->pawnJumpSquares)
                    & emptySquares;

                // Intersect the pushes with the current checkMask. The
                // push targets are now legal.
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
                // Generate left and right attack moves for free low pawns.
                // All legal aggressive targets one square ahead and
                // to the right.
                uint64_t ar =
                    shift<x->upRight>(freeLowPawns & x->notRightCol)
                        & enemies;

                // All legal aggressive targets one square ahead and
                // to the left.
                uint64_t al =
                    shift<x->upLeft>(freeLowPawns & x->notLeftCol)
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

            // Generate single and double pushes for pinned low pawns,
            // if any.
            // Generate left and right attack moves for pinned low pawns,
            // if any.
            if (pinnedLowPawns) {
                if(FT != Aggressive) {
                    // All pseudo-legal, passive targets one square ahead.
                    uint64_t p1 = shift<x->up>(pinnedLowPawns) & emptySquares;

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
                        shift<x->upRight>(pinnedLowPawns & x->notRightCol)
                            & enemies;

                    // All pseudo-legal aggressive targets one square ahead
                    // and to the left.
                    uint64_t al =
                        shift<x->upLeft>(pinnedLowPawns & x->notLeftCol)
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

            // Generate promotion moves for free high pawns.
            if (freeHighPawns) {
                if (FT != Aggressive) {
                    // Calculate single promotion push.
                    // All legal promotion targets one square ahead.
                    uint64_t p1 = shift<x->up>(freeHighPawns) &
                                  emptySquares & checkMask;

                    // Make promotion moves from single push.
                    for(int d; p1; p1 &= p1 - 1){
                        d = bitScanFwd(p1);
                        moves = makePromotions(moves, d + x->down, d);
                    }
                }

                if (FT != Passive) {
                    // All legal aggressive targets one square ahead
                    // and to the right.
                    uint64_t ar =
                        shift<x->upRight>(freeHighPawns & x->notRightCol)
                            & enemies;

                    // All legal aggressive targets one square ahead
                    // and to the left.
                    uint64_t al =
                        shift<x->upLeft >(freeHighPawns & x->notLeftCol)
                            & enemies;

                    // Make moves from aggressive right targets.
                    for (int d; ar; ar &= ar - 1) {
                        d = bitScanFwd(ar);
                        moves = makePromotions(moves, d + x->downLeft, d);
                    }

                    // Make moves from aggressive left targets.
                    for (int d; al; al &= al - 1) {
                        d = bitScanFwd(al);
                        moves = makePromotions(moves, d + x->downRight, d);
                    }
                }
            }

            // Generate promotion moves for pinned high pawns.
            if (pinnedHighPawns) {
                if (FT != Aggressive) {
                    // Calculate single promotion push for pinned pawns.
                    // All pseudo-legal promotion targets one square ahead.
                    uint64_t p1 =
                        shift<x->up>(pinnedHighPawns) & emptySquares & checkMask;

                    // Make legal promotion moves from pseudo-legal targets.
                    for (int o, d; p1; p1 &= p1 - 1){
                        d = bitScanFwd(p1);
                        o = d + x->down;
                        if (rayBoard(kingSquare, o) &
                            p1 & (uint64_t) - (int64_t)p1)
                            moves = makePromotions(moves, o, d);
                    }
                }

                if (FT != Passive) {
                    // All pseudo-legal aggressive targets one square ahead
                    // and to the right.
                    uint64_t ar =
                        shift<x->upRight>(pinnedHighPawns & x->notRightCol)
                            & enemies;

                    // All pseudo-legal aggressive targets one square ahead
                    // and to the left.
                    uint64_t al =
                        shift<x->upLeft >(pinnedHighPawns & x->notLeftCol)
                            & enemies;

                    // Make legal promotion moves from aggressive right targets.
                    for (int o, d; ar; ar &= ar - 1) {
                        d = bitScanFwd(ar);
                        o = d + x->downLeft;
                        if (rayBoard(kingSquare, o) &
                            ar & (uint64_t) - (int64_t)ar)
                            moves = makePromotions(moves, o, d);
                    }

                    // Make legal promotion moves from aggressive left targets.
                    for (int o, d; al; al &= al - 1) {
                        d = bitScanFwd(al);
                        o = d + x->downRight;
                        if (rayBoard(kingSquare, o) &
                            al & (uint64_t) - (int64_t)al)
                            moves = makePromotions(moves, o, d);
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
            const uint64_t eppBoard  = SquareToBitBoard[enPassantSquare];

            // The en passant destination board.
            const uint64_t destBoard = shift<x->up>(eppBoard);

            // If we are in single check and the destination
            // doesn't block... and if the en passant pawn is
            // not the king's attacker... Don't generate an en
            // passant move.
            if (!(destBoard & checkMask) &&
                !(eppBoard  & SquareToPawnAttacks[us][kingSquare]))
                    return moves;

            // Calculate the pass mask.
            const uint64_t passMask =
                    shift<x->right>(eppBoard & x->notRightCol) |
                    shift<x->left >(eppBoard & x->notLeftCol );

            // Calculate free passing pawns.
            const uint64_t freePasses   = passMask & freeLowPawns;

            // Calculate pinned passing pawns.
            const uint64_t pinnedPasses = passMask & pinnedLowPawns;

            // If there is a passing pawn, generate legal
            // en passant moves.
            if (!(freePasses || pinnedPasses))
                return moves;

            // If the king is on the en passant rank then
            // a horizontal en passant discovered check is possible.
            if (king & x->enPassantRank) {
                // Find the snipers on the en passant rank.
                const uint64_t snipers =
                        (board->getPieces<them, Queen>() |
                         board->getPieces<them, Rook>()) &
                         x->enPassantRank;

                // Check to see if the en passant pawn
                // (and attacking pawn) are between any of
                // the snipers and the king square. If so,
                // check to see if these two pawns are the
                // ONLY blocking pieces between the sniper
                // and the king square.
                // If this is the case, then any en passant move
                // will leave our king in check. We are done
                // generating pawn moves.
                for (uint64_t s = snipers; s; s &= s - 1) {
                    const uint64_t path =
                            pathBoard(bitScanFwd(s), kingSquare);
                    if (eppBoard & path) {
                        const uint64_t b = board->getAllPieces() &
                            ~snipers & path, c = b & (b - 1);
                        if (b && c && !(c & (c - 1)))
                            return moves;
                    }
                }

            // If the king square has a path to the en Passant
            // square but isn't on the rank of the en passant
            // pawn, en en passant discovered check is still
            // possible.
            } else if (pathBoard(kingSquare, enPassantSquare)) {

                // Find the diagonal snipers.
                const uint64_t snipers =
                        (board->getPieces<them, Queen>() |
                         board->getPieces<them, Bishop>()) &
                         attackBoard<Bishop>(0, kingSquare);

                // Check to see if the en passant pawn
                // is between any of the snipers and the king
                // square. If so, check to see if this pawn is
                // the ONLY blocking piece between the sniper
                // and the king square.
                // If this is the case, then any en passant move
                // will leave our king in check. We are done
                // generating pawn moves.
                for (uint64_t s = snipers; s; s &= s - 1) {
                    const uint64_t path =
                            pathBoard(bitScanFwd(s), kingSquare);
                    if (eppBoard & path) {
                        const uint64_t b = board->getAllPieces() & path;
                        if (b && !(b & (b - 1)))
                            return moves;
                    }
                }
            }

            // Calculate the destination square.
            const int destinationSquare = enPassantSquare + x->up;

            // Add free-pass en passant moves.
            for(uint64_t fp = freePasses; fp; fp &= fp - 1) {
                const int o = bitScanFwd(fp);
                *moves++ = Move::make<EnPassant>(o, destinationSquare);
            }

            // Add pinned-pass en passant moves.
            for(uint64_t pp = pinnedPasses; pp; pp &= pp - 1) {
                const int o = bitScanFwd(pp);
                if(destBoard & rayBoard(kingSquare, o))
                    *moves++ =
                        Move::make<EnPassant>(o, destinationSquare);
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
            constexpr const Defaults* const x = defaults<us>();

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
                const uint64_t theirQueens = board->getPieces<them, Queen>();

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
                    const uint64_t blocker = pathBoard(ssq, ksq) & allPieces;
                    if(blocker && !(blocker & (blocker - 1))) blockers |= blocker;
                }

                // Determine which friendly pieces block sliding attacks on our
                // king.
                // If our king is in single check, determine the path between the
                // king and his attacker.
                const uint64_t kingGuard = ourPieces & blockers,
                               checkPath = (checkType == Check ? pathBoard(
                                      ksq, bitScanFwd(checkBoard)
                               ) | checkBoard : FullBoard),
                // A filter to limit all pieces to blocking
                // moves only in the event of single check
                // while simultaneously limiting generation
                // to "aggressive", "passive", or "all" move
                // types.
                               fullFilter = partialFilter & checkPath;

                // Make non-king moves.
                moves = makePawnMoves<us, FT>(board, checkPath, kingGuard,  ksq, moves);
                moves = makeMoves<us,   Rook>(board, kingGuard, fullFilter, ksq, moves);
                moves = makeMoves<us, Knight>(board, kingGuard, fullFilter, ksq, moves);
                moves = makeMoves<us, Bishop>(board, kingGuard, fullFilter, ksq, moves);
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
            if(!(x->kingSideMask & allPieces) &&
                board->hasCastlingRights<us, KingSide>() &&
                safeSquares<us>(board, x->kingSideCastlePath))
                *moves++ = Move::make<Castling>(
                        ksq, x->kingSideDestination
                );

            // Generate queen-side castle.
            if(!(x->queenSideMask & allPieces) &&
                board->hasCastlingRights<us, QueenSide>() &&
                safeSquares<us>(board, x->queenSideCastlePath))
                *moves++ = Move::make<Castling>(
                        ksq, x->queenSideDestination
                );

            return (moves - initialMove);
        }
    } // namespace (anon)

    namespace MoveFactory {
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