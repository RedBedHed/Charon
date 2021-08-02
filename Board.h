//
// Created by evcmo on 6/8/2021.
//

#ifndef CHARON_BOARD_H
#define CHARON_BOARD_H
#include <iostream>
#include <ostream>
#include <cstdint>
#include <locale>
#include "ChaosMagic.h"
#include "Move.h"

namespace Charon {

    using namespace Witchcraft;
    using std::ostream;

    /**
     * <summary>
     * A struct to hold all directions and masks
     * for an Alliance.
     * </summary>
     *
     * @struct Defaults
     */
    struct Defaults final {
    public:
        const Direction up;
        const Direction upRight;
        const Direction upLeft;
        const Direction down;
        const Direction downRight;
        const Direction downLeft;
        const Direction left;
        const Direction right;
        const uint64_t  enPassantRank;
        const uint64_t  notRightCol;
        const uint64_t  notLeftCol;
        const uint64_t  pawnStart;
        const uint64_t  pawnJumpSquares;
        const uint64_t  kingSideMask;
        const uint64_t  queenSideMask;
        const uint64_t  kingSideDestination;
        const uint64_t  queenSideDestination;
        const uint64_t  kingSideRookOrigin;
        const uint64_t  queenSideRookOrigin;
        const uint64_t  kingSideRookDestination;
        const uint64_t  queenSideRookDestination;
        const uint64_t  kingSideRookMoveMask;
        const uint64_t  queenSideRookMoveMask;
    };

    /** Default directions and masks for White. */
    constexpr Defaults WhiteDefaults = {
            North,
            NorthEast,
            NorthWest,
            South,
            SouthEast,
            SouthWest,
            West,
            East,
            WhiteEnPassantRank,
            NotEastFile,
            NotWestFile,
            WhitePawnsStartPosition,
            WhitePawnJumpSquares,
            WhiteKingsideMask,
            WhiteQueensideMask,
            WhiteKingsideKingDestination,
            WhiteQueensideKingDestination,
            WhiteKingsideRookOrigin,
            WhiteQueensideRookOrigin,
            WhiteKingsideRookDestination,
            WhiteQueensideRookDestination,
            WhiteKingsideRookMask,
            WhiteQueensideRookMask,
    };

    /** Default directions and masks for Black. */
    constexpr Defaults BlackDefaults = {
            South,
            SouthWest,
            SouthEast,
            North,
            NorthWest,
            NorthEast,
            East,
            West,
            BlackEnPassantRank,
            NotWestFile,
            NotEastFile,
            BlackPawnsStartPosition,
            BlackPawnJumpSquares,
            BlackKingsideMask,
            BlackQueensideMask,
            BlackKingsideKingDestination,
            BlackQueensideKingDestination,
            BlackKingsideRookOrigin,
            BlackQueensideRookOrigin,
            BlackKingsideRookDestination,
            BlackQueensideRookDestination,
            BlackKingsideRookMask,
            BlackQueensideRookMask,
    };

    /**
     * <summary>
     * A struct to keep track of the board state, for use in
     * applying and retracting moves.
     * </summary>
     *
     * @struct State
     */
    class State final {
    private:
        /**
         * @private
         * Board is State's best bud.
         */
        friend class Board;

        /**
         * @private
         * The castling rights for this State.
         */
        uint8_t castlingRights;

        /**
         * @private
         * The en passant square for this State.
         */
        Square epSquare;

        /**
         * @private
         * The previous State.
         */
        State* prevState;

        /**
         * @private
         * The captured piece for this State.
         */
        PieceType capturedPiece;
    public:

        /**
         * A default constructor for this State.
         */
        constexpr State() :
        castlingRights(0x0FU),
        epSquare(NullSQ),
        prevState(nullptr),
        capturedPiece(NullPT)
        {  }
    };

    /**
     * <summary>
     *  <p>
     * A board is a mutable data structure that consists of
     * layered piece bitboards and an added piece mailbox.
     * The bitboards allow for speedy move generation, while
     * the mailbox ensures fast Move application and retraction.
     * Furthermore, the mailbox allows Move data to be condensed
     * into just 16 bits, improving memory efficiency during
     * deep searches.
     *  </p>
     * </summary>
     *
     * @class Board
     * @author Ellie Moore
     * @version 08.01.2021
     */
    class Board final {
    private:

        /**
         * @private
         * Bitboard layers for the various piece types.
         */
        uint64_t pieces[2][7]{};

        /**
         * @private
         * All piece bitboards sandwiched together.
         */
        uint64_t allPieces;

        /**
         * @private
         * The alliance of the current player.
         */
        Alliance currentPlayerAlliance;

        /**
         * @private
         * A mailbox representation of this board.
         */
        PieceType mailbox[BoardLength]{
            NullPT, NullPT, NullPT, NullPT, NullPT, NullPT, NullPT, NullPT,
            NullPT, NullPT, NullPT, NullPT, NullPT, NullPT, NullPT, NullPT,
            NullPT, NullPT, NullPT, NullPT, NullPT, NullPT, NullPT, NullPT,
            NullPT, NullPT, NullPT, NullPT, NullPT, NullPT, NullPT, NullPT,
            NullPT, NullPT, NullPT, NullPT, NullPT, NullPT, NullPT, NullPT,
            NullPT, NullPT, NullPT, NullPT, NullPT, NullPT, NullPT, NullPT,
            NullPT, NullPT, NullPT, NullPT, NullPT, NullPT, NullPT, NullPT,
            NullPT, NullPT, NullPT, NullPT, NullPT, NullPT, NullPT, NullPT
        };

        /**
         * @private
         * A pointer to an externally-allocated state
         * object to optimize move retraction.
         */
        State* currentState;
    public:

        /**
         * A method to expose the current player's alliance.
         *
         * @return the current player's alliance
         */
        [[nodiscard]]
        constexpr Alliance currentPlayer() const
        { return currentPlayerAlliance; }

        /**
         * A method to expose each piece bitboard.
         *
         * @tparam A the alliance of the bitboard
         * @tparam PT the piece type of the bitboard
         * @return a piece bitboard
         */
        template<Alliance A, PieceType PT>
        constexpr uint64_t getPieces()
        { return pieces[A][PT]; }

        /**
         * A method to expose a half-sandwich bitboard
         * of all piece bitboards belonging to the given
         * alliance.
         *
         * @tparam A the alliance of the bitboard
         * @return a bitboard of all pieces belonging
         * to the given alliance
         */
        template<Alliance A>
        constexpr uint64_t getPieces()
        { return pieces[A][NullPT]; }

        /**
         * A method to expose the current State's castling
         * rights.
         *
         * @tparam A the alliance
         * @tparam CT the castle type
         * @return the castling rights of the given type and
         * alliance
         */
        template <Alliance A, CastleType CT>
        [[nodiscard]]
        constexpr bool hasCastlingRights() const {
            static_assert(A == White || A == Black);
            static_assert(CT == KingSide || CT == QueenSide);
            return A == White ?
                   CT == KingSide ?
                   currentState->castlingRights & 0x02U :
                   currentState->castlingRights & 0x01U :
                   CT == KingSide ?
                   currentState->castlingRights & 0x08U :
                   currentState->castlingRights & 0x04U ;
        }

        /**
         * A method to pop each bit off of the given bitboard,
         * inserting the given character into the corresponding
         * index of the given buffer.
         *
         * @param buffy the buffer to fill
         * @param b the bitboard to pop
         * @param c the character to use
         */
        static constexpr void
        popTo(char* const buffy, uint64_t b, const char c)
        { for (; b > 0; b &= b - 1) buffy[bitScanFwd(b)] = c; }

        /**
         * A method to represent this board with a string.
         *
         * @return a string representing this board
         */
        [[nodiscard]]
        inline std::string toString() const {
            char buffer[BoardLength]{};
            popTo(buffer, pieces[White][Pawn]  , 'P');
            popTo(buffer, pieces[White][Rook]  , 'R');
            popTo(buffer, pieces[White][Knight], 'N');
            popTo(buffer, pieces[White][Bishop], 'B');
            popTo(buffer, pieces[White][Queen] , 'Q');
            popTo(buffer, pieces[White][King]  , 'K');
            popTo(buffer, pieces[Black][Pawn]  , 'p');
            popTo(buffer, pieces[Black][Rook]  , 'r');
            popTo(buffer, pieces[Black][Knight], 'n');
            popTo(buffer, pieces[Black][Bishop], 'b');
            popTo(buffer, pieces[Black][Queen] , 'q');
            popTo(buffer, pieces[Black][King]  , 'k');
            std::string sb;
            sb.reserve(136);
            sb.append("\n\t    H   G   F   E   D   C   B   A");
            sb.append(
                "\n\t  +---+---+---+---+---+---+---+---+\n"
            );
            int x = 0;
            for(char i = '1'; i < '9'; ++i) {
                sb.push_back('\t');
                sb.push_back(i);
                sb.append(" | ");
                for(char j = '1'; j < '9'; ++j) {
                    char c = buffer[x++];
                    sb.push_back(c == '\0' ? ' ' : c);
                    sb.append(" | ");
                }
                sb.push_back(i);
                sb.append(
                    "\n\t  +---+---+---+---+---+---+---+---+\n"
                );
            }
            sb.append("\t    H   G   F   E   D   C   B   A\n");
            return sb;
        }

        /**
         * An overloaded insertion operator for use in printing
         * a string representation of this board.
         *
         * @param out the output stream to use
         * @param in the board to stringify and print
         * @return a reference to the output stream, for chaining
         * purposes
         */
        friend ostream& operator<<(ostream& out, const Board& in) {
            return out << in.toString();
        }

        /** Deleted copy constructor. */
        Board(const Board&) = delete;

        /** Deleted move constructor. */
        Board(Board&&) = delete;

        /**
         * <summary>
         *  <p><br/>
         * A builder pattern for the Board class to allow
         * the client flexibility and readability during
         * the instantiation of a Board Object.
         *  </p>
         * </summary>
         * @class Builder
         * @author Ellie Moore
         * @version 06.07.2021
         */
        class Builder final {
        private:

            /**
             * @private
             * Board is Builder's bestie.
             */
            friend class Board;

            /**
             * @private
             * The alliance of the player to start.
             */
            Alliance currentPlayerAlliance;

            /**
             * @private
             * An array to hold each piece bitboard.
             */
            uint64_t pieces[2][6]{{
                WhitePawnsStartPosition,
                WhiteRooksStartPosition,
                WhiteKnightsStartPosition,
                WhiteBishopsStartPosition,
                WhiteQueenStartPosition,
                WhiteKingStartPosition
            },{
                BlackPawnsStartPosition,
                BlackRooksStartPosition,
                BlackKnightsStartPosition,
                BlackBishopsStartPosition,
                BlackQueenStartPosition,
                BlackKingStartPosition
            }};

            /**
             * @private
             * The initial state of the board under
             * construction.
             */
            State* state;
        public:

            /**
             * @public
             * A public constructor for a Builder.
             *
             * @param s the initial state of the board
             * under construction.
             */
            explicit constexpr Builder(State& s) :
                    currentPlayerAlliance(White),
                    state(&s)
            {  }

            /**
             * @public
             * A public constructor to copy a board into
             * this Builder.
             *
             * @param board the board to copy
             */
            explicit constexpr Builder(const Board& board) :
            currentPlayerAlliance(~board.currentPlayerAlliance),
            state(board.currentState) {
                pieces[White][Pawn]    = board.pieces[White][Pawn];
                pieces[White][Rook]    = board.pieces[White][Rook] ;
                pieces[White][Knight]  = board.pieces[White][Knight];
                pieces[White][Bishop]  = board.pieces[White][Bishop];
                pieces[White][Queen]   = board.pieces[White][Queen];
                pieces[White][King]    = board.pieces[White][King];
                pieces[Black][Pawn]    = board.pieces[Black][Pawn];
                pieces[Black][Rook]    = board.pieces[Black][Rook];
                pieces[Black][Knight]  = board.pieces[Black][Knight];
                pieces[Black][Bishop]  = board.pieces[Black][Bishop];
                pieces[Black][Queen]   = board.pieces[Black][Queen];
                pieces[Black][King]    = board.pieces[Black][King];
            }

            /**
             * A method to set the entire bitboard for a given
             * alliance and piece type.
             *
             * @tparam A the alliance of the bitboard to set
             * @tparam PT the piece type of the bitboard to set
             * @param p the piece bitboard to use
             * @return a reference to the instance
             */
            template <Alliance A, PieceType PT>
            inline Builder& setPieces(const uint64_t p)
            { pieces[A][PT] = p; return *this; }

            /**
             * A method to set a single piece onto the bitboard
             * of the given alliance and piece type.
             *
             * @tparam A the alliance of the piece to set
             * @tparam PT the type of the piece to set
             * @param sq the square on which to set the piece
             * @return a reference to the instance
             */
            template <Alliance A, PieceType PT>
            inline Builder& setPiece(const int sq)
            { pieces[A][PT] |= SquareToBitBoard[sq]; return *this; }

            /**
             * A method to set the en passant square of the board
             * under construction.
             *
             * @param square the integer index of the en passant
             * square
             * @return a reference to the instance
             */
            inline Builder& setEnPassantSquare(const Square square)
            { state->epSquare = square; return *this; }

            /**
             * A method to set the castling rights of the initial
             * state of the board under construction.
             *
             * @tparam A the alliance for which to set castling
             * rights
             * @tparam CT the castle type for which to set castling
             * rights
             * @tparam B the rights
             * @return a reference to the instance
             */
            /*template <Alliance A, CastleType CT, bool B>
            inline Builder& setCastlingRights() {
                static_assert(A == White || A == Black);
                static_assert(CT == KingSide || CT == QueenSide);
                !B ?
                    state->castlingRights &= A == White?
                        CT == KingSide ?
                            0x0DU : 0x0EU :
                        CT == KingSide ?
                            0x07U : 0x0BU :
                    state->castlingRights |= A == White?
                        CT == KingSide ?
                            0x02U : 0x01U :
                        CT == KingSide ?
                            0x08U : 0x04U ;
                return *this;
            }*/

            /**
             * A method to set the alliance of the initial
             * current player for the board under construction.
             *
             * @tparam A the alliance of the initial current
             * player
             * @return a reference to the instance
             */
            template <Alliance A>
            inline Builder& setCurrentPlayer()
            { currentPlayerAlliance = A; return *this; }

            /**
             * A method to instantiate a board from the data
             * stored in this Builder.
             *
             * @return a reference to the instance
             */
            [[nodiscard]]
            constexpr Board build() const { return Board(*this); }

            /** A deleted copy constructor. */
            Builder(const Builder&) = delete;

            /** A deleted move constructor. */
            Builder(Builder&&) = delete;
        };
    private:

        /**
         * @private
         * @static
         * A method to sandwich piece bitboards into a single
         * bitboard for the given alliance.
         *
         * @param b the Builder to use
         * @return a sandwich of the given Alliance's piece
         * boards.
         */
        template<Alliance A>
        static constexpr uint64_t sandwich(const Builder& b) {
            return b.pieces[A][Pawn]   | b.pieces[A][Rook]   |
                   b.pieces[A][Knight] | b.pieces[A][Bishop] |
                   b.pieces[A][Queen]  | b.pieces[A][King];
        }

        /**
         * @private
         * @static
         * A method to initialize each piece bitboard for the
         * given alliance.
         *
         * @param b the Builder to use
         */
        template<Alliance A> static constexpr void
        initPieceBoards(uint64_t* const p, const Builder& b) {
            p[Pawn]   = b.pieces[A][Pawn];
            p[Rook]   = b.pieces[A][Rook];
            p[Knight] = b.pieces[A][Knight];
            p[Bishop] = b.pieces[A][Bishop];
            p[Queen]  = b.pieces[A][Queen];
            p[King]   = b.pieces[A][King];
            p[NullPT] = sandwich<A>(b);
        }

        explicit constexpr Board(const Builder& b) :
        allPieces(0),
        currentPlayerAlliance(b.currentPlayerAlliance),
        currentState(b.state) {
            initPieceBoards<White>(pieces[White], b);
            initPieceBoards<Black>(pieces[Black], b);
            for (int j = Pawn; j < NullPT; ++j) {
                for (uint64_t x = b.pieces[White][j]; x; x &= x - 1)
                    mailbox[bitScanFwd(x)] = (PieceType) j;
                for (uint64_t x = b.pieces[Black][j]; x; x &= x - 1)
                    mailbox[bitScanFwd(x)] = (PieceType) j;
            }
            allPieces =
                pieces[White][NullPT] | pieces[Black][NullPT];
        }

        template<Alliance A>
        constexpr void applyMove(const Move& m, State& state) {
            // ASSUME THAT THE MOVE IS LEGAL ! ! !
            assert(currentState != &state);
            state.prevState = currentState;
            currentState = &state;
            const int origin    = m.origin(),
                    destination = m.destination(),
                    isPromotion = m.isPromotion();
            PieceType captureType = mailbox[destination],
                    activeType  = mailbox[origin];
            currentState->capturedPiece = captureType;
            constexpr const Alliance us = A, them = ~us;
            const uint64_t originBoard      = SquareToBitBoard[origin],
                    destinationBoard = SquareToBitBoard[destination],
                    moveBB           = originBoard | destinationBoard;
            constexpr const Defaults* const x = us == White?
                                      &WhiteDefaults: &BlackDefaults;
            mailbox[origin] = NullPT;
            currentState->castlingRights =
                    currentState->prevState->castlingRights;
            currentPlayerAlliance = them;
            if(isPromotion) {
                pieces[us][activeType]    ^= moveBB;
                pieces[us][NullPT]        ^= moveBB;
                pieces[them][NullPT]      &= ~destinationBoard;
                pieces[them][captureType] &= ~destinationBoard;
                allPieces = pieces[us][NullPT] | pieces[them][NullPT];
                mailbox[destination] = PieceType(m.promotionPiece());
                return;
            }
            mailbox[destination] = activeType;
            const int moveType = m.moveType();
            if(moveType == FreeForm || moveType == PawnJump) {
                if(activeType == Rook) {
                    if(x->kingSideRookOrigin == originBoard)
                        currentState->castlingRights &= us == White? 0x0EU : 0x0DU;
                    else if(x->queenSideRookOrigin == originBoard)
                        currentState->castlingRights &= us == White? 0x0BU : 0x07U;
                } else if(activeType == King) {
                    currentState->castlingRights &= us == White? 0x0CU: 0x03U;
                }
                pieces[us][activeType] ^= moveBB;
                pieces[us][NullPT]     ^= moveBB;
                if(captureType != NullPT) {
                    pieces[them][NullPT]      &= ~destinationBoard;
                    pieces[them][captureType] &= ~destinationBoard;
                }
                allPieces = pieces[us][NullPT] | pieces[them][NullPT];
                currentState->epSquare =
                        moveType == PawnJump ? (Square) destination : NullSQ;
                return;
            }
            if(moveType == Castling) {
                currentState->castlingRights &= us == White? 0x0CU: 0x03U;
                uint64_t rookMoveBB;
                if(x->kingSideMask & destinationBoard) {
                    rookMoveBB = x->kingSideRookMoveMask;
                    mailbox[x->kingSideRookOrigin] = NullPT;
                    mailbox[x->kingSideRookDestination] = Rook;
                } else {
                    rookMoveBB = x->queenSideRookMoveMask;
                    mailbox[x->queenSideRookOrigin] = NullPT;
                    mailbox[x->queenSideRookDestination] = Rook;
                }
                const uint64_t fullBB = moveBB | rookMoveBB;
                pieces[us][Rook]       ^= rookMoveBB;
                pieces[us][activeType] ^= moveBB;
                pieces[us][NullPT]     ^= fullBB;
                allPieces              ^= fullBB;
                return;
            }
            const int epSquare = currentState->prevState->epSquare;
            const uint64_t captureBB = SquareToBitBoard[epSquare];
            pieces[us][Pawn]   ^= moveBB;
            pieces[us][NullPT] ^= moveBB;
            pieces[them][Pawn] ^= captureBB;
            pieces[them][NullPT] ^= captureBB;
            allPieces = pieces[us][NullPT] | pieces[them][NullPT];
            mailbox[epSquare] = NullPT;
        }

        template<Alliance A>
        constexpr void retractMove(const Move& m) {
            // ASSUME THAT THE MOVE IS LEGAL ! ! !
            constexpr const Alliance us = A, them = ~us;
            const int origin      = m.origin(),
                      destination = m.destination(),
                      isPromotion = m.isPromotion();
            PieceType captureType = currentState->capturedPiece,
                      activeType  = mailbox[destination];
            const uint64_t originBoard      = SquareToBitBoard[origin],
                           destinationBoard = SquareToBitBoard[destination],
                           moveBB           = originBoard | destinationBoard;
            constexpr const Defaults* const x = us == White?
                    &WhiteDefaults: &BlackDefaults;
            currentPlayerAlliance = us;
            mailbox[origin] = activeType;
            mailbox[destination] = captureType;
            if(isPromotion) {
                pieces[us][activeType] ^= moveBB;
                pieces[us][NullPT]     ^= moveBB;
                if(captureType != NullPT) {
                    pieces[them][NullPT]      |= destinationBoard;
                    pieces[them][captureType] |= destinationBoard;
                }
                allPieces = pieces[us][NullPT] | pieces[them][NullPT];
                currentState = currentState->prevState;
                return;
            }
            const int moveType = m.moveType();
            if(moveType == FreeForm || moveType == PawnJump) {
                pieces[us][activeType]    ^= moveBB;
                pieces[us][NullPT]        ^= moveBB;
                if(captureType != NullPT) {
                    pieces[them][NullPT]      |= destinationBoard;
                    pieces[them][captureType] |= destinationBoard;
                }
                allPieces = pieces[us][NullPT] | pieces[them][NullPT];
            }
            else if(moveType == Castling) {
                uint64_t rookMoveBB;
                if(x->kingSideMask & destinationBoard) {
                    rookMoveBB = x->kingSideRookMoveMask;
                    mailbox[x->kingSideRookOrigin] = Rook;
                    mailbox[x->kingSideRookDestination] = NullPT;
                } else {
                    rookMoveBB = x->queenSideRookMoveMask;
                    mailbox[x->queenSideRookOrigin] = Rook;
                    mailbox[x->queenSideRookDestination] = NullPT;
                }
                const uint64_t fullBB = moveBB | rookMoveBB;
                pieces[us][Rook]       ^= rookMoveBB;
                pieces[us][activeType] ^= moveBB;
                pieces[us][NullPT]     ^= fullBB;
                allPieces              ^= fullBB;
            }
            else {
                const uint64_t epSquare  = currentState->prevState->epSquare;
                const uint64_t captureBB = SquareToBitBoard[epSquare];
                pieces[us][Pawn]     ^= moveBB;
                pieces[us][NullPT]   ^= moveBB;
                pieces[them][NullPT] |= captureBB;
                pieces[them][Pawn] |= captureBB;
                allPieces = pieces[us][NullPT] | pieces[them][NullPT];
                mailbox[epSquare] = Pawn;
            }
            currentState = currentState->prevState;
        }

    public:

        [[nodiscard]]
        constexpr uint64_t getAllPieces() const
        { return allPieces; }

        [[nodiscard]]
        constexpr int getEpSquare() const
        { return currentState->epSquare; }

        [[nodiscard]]
        constexpr PieceType getPiece(const int square) const
        { return mailbox[square]; }

        constexpr void applyMove(const Move& m, State& s) {
            return currentPlayerAlliance == White?
                   applyMove<White>(m, s) :
                   applyMove<Black>(m, s);
        }

        constexpr void retractMove(const Move& m) {
            return currentPlayerAlliance == White ?
                   retractMove<Black>(m) :
                   retractMove<White>(m);
        }
    };

}

#endif //CHARON_BOARD_H
