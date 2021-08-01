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
     * for an alliance.
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
     * A function to get the defaults for a given Alliance.
     *
     * @tparam A the Alliance to get the defaults for
     * @return a pointer to the defaults for the given
     * Alliance
     */
    template <Alliance A>
    constexpr const Defaults* getDefaults()
    { return A == White ? &WhiteDefaults : &BlackDefaults; }

    /**
     * <summary>
     * A struct to keep track of the board state, for use in
     * applying and retracting moves.
     * </summary>
     *
     * @struct State
     */
    struct State final {
    private:

        /**
         * The castling rights for this State.
         */
        uint8_t castlingRights;
    public:

        /**
         * The en passant square for this State.
         */
        Square epSquare;

        /**
         * The previous State.
         */
        State* prevState;

        /**
         * The captured piece for this State.
         */
        PieceType capturedPiece;

        /**
         * A default constructor for this State.
         */
        constexpr State() :
        castlingRights(0x0FU),
        epSquare(NullSQ),
        prevState(nullptr),
        capturedPiece(NullPT)
        {  }

        /**
         * A method to get the castling rights for a
         * given alliance and piece type.
         *
         * @tparam A the alliance to consider
         * @tparam CT the castling type to consider
         * @return whether or not the given alliance
         * iis allowed to castle on the side specified
         * by the castling type
         */
        template <Alliance A, CastleType CT>
        constexpr bool getCastlingRights() {
            return A == White ?
                      CT == KingSide ?
                         (castlingRights >> 1U) & 1U :
                          castlingRights        & 1U :
                      CT == KingSide ?
                         (castlingRights >> 3U) & 1U :
                         (castlingRights >> 2U) & 1U ;
        }

        /**
         * A method to set the castling rights for a
         * given alliance and piece type, with the
         * flag specified at compile time.
         *
         * @tparam A the alliance to set rights for
         * @tparam CT the castling type to set rights for
         * @tparam B the flag to use
         */
        template <Alliance A, CastleType CT, bool B>
        constexpr void setCastlingRights() {
            castlingRights = A == White ?
                   CT == KingSide ?
                   (B << 1U) | (castlingRights & 0x0DU) :
                           B | (castlingRights & 0x0EU) :
                   CT == KingSide ?
                   (B << 3U) | (castlingRights & 0x07U) :
                   (B << 2U) | (castlingRights & 0x0BU) ;
        }

        /**
         * A method to set the castling rights for a
         * given alliance and piece type, with the
         * flag specified at run time.
         *
         * @tparam A the alliance to set rights for
         * @tparam CT the castling type to set rights for
         * @tparam B the flag to use
         */
        template <Alliance A, CastleType CT>
        constexpr void setCastlingRights(const bool B) {
            castlingRights = A == White ?
                   CT == KingSide ?
                   (B << 1U) | (castlingRights & 0x0DU) :
                           B | (castlingRights & 0x0EU) :
                   CT == KingSide ?
                   (B << 3U) | (castlingRights & 0x07U) :
                   (B << 2U) | (castlingRights & 0x0BU) ;
        }
    };

    class Board;

    class Player final {
    private:

        friend class Board;
        uint64_t pieces[7]{};
        uint64_t allPieces;
        uint64_t white;
    public:

        constexpr Player(const uint64_t* const p,
                         const uint64_t allPieces,
                         const bool isWhite) :
                allPieces(allPieces),
                white(isWhite) {
            pieces[Pawn] = p[Pawn];
            pieces[Rook] = p[Rook];
            pieces[Knight] = p[Knight];
            pieces[Bishop] = p[Bishop];
            pieces[Queen] = p[Queen];
            pieces[King] = p[King];
            pieces[NullPT] = 0;
        }

        constexpr Player(const Player&) = default;

        [[nodiscard]]
        constexpr bool isWhite() const
        { return white; }

        template <PieceType PT>
        [[nodiscard]]
        constexpr uint64_t getPieces() const
        { return pieces[PT]; }

        [[nodiscard]]
        constexpr uint64_t getAllPieces() const
        { return allPieces; }
    };

    class Board final {
    private:

        friend class Player;

        Player whitePlayer;
        Player blackPlayer;
        uint64_t allPieces;
        Alliance currentPlayerAlliance;
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
        State* currentState;
    public:

        [[nodiscard]]
        constexpr Alliance currentPlayer() const
        { return currentPlayerAlliance; }

        template<Alliance A>
        [[nodiscard]] inline Player* getPlayer()
        { return A == White? &whitePlayer: &blackPlayer; }

        [[nodiscard]] inline Player* getPlayer(const Alliance A)
        { return A == White? &whitePlayer: &blackPlayer; }

        static inline void popTo(char* const arr,
                                 uint64_t b,
                                 const char c)
        { for (; b > 0; b &= b - 1) arr[bitScanFwd(b)] = c; }

        template<Alliance A, PieceType PT>
        constexpr uint64_t getPieces()
        { return A == White? whitePlayer.pieces[PT]: blackPlayer.pieces[PT]; }

        template<Alliance A>
        constexpr uint64_t getPieces()
        { return A == White? whitePlayer.allPieces: blackPlayer.allPieces; }

        template <Alliance A, CastleType CT>
        [[nodiscard]]
        constexpr bool hasCastlingRights() const {
            static_assert(A == White || A == Black);
            static_assert(CT == KingSide || CT == QueenSide);
            return currentState->getCastlingRights<A, CT>();
        }

        [[nodiscard]]
        inline std::string toString() const {
            char buffer[BoardLength]{};
            popTo(buffer, whitePlayer.pieces[Queen] , 'Q');
            popTo(buffer, whitePlayer.pieces[King]  , 'K');
            popTo(buffer, whitePlayer.pieces[Rook]  , 'R');
            popTo(buffer, whitePlayer.pieces[Bishop], 'B');
            popTo(buffer, whitePlayer.pieces[Knight], 'N');
            popTo(buffer, whitePlayer.pieces[Pawn]  , 'P');
            popTo(buffer, blackPlayer.pieces[Queen] , 'q');
            popTo(buffer, blackPlayer.pieces[King]  , 'k');
            popTo(buffer, blackPlayer.pieces[Rook]  , 'r');
            popTo(buffer, blackPlayer.pieces[Bishop], 'b');
            popTo(buffer, blackPlayer.pieces[Knight], 'n');
            popTo(buffer, blackPlayer.pieces[Pawn]  , 'p');
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

        friend ostream& operator<<(ostream& out, const Board& in) {
            return out << in.toString();
        }

        Board(const Board&) = default;

        class Builder final {
        private:

            friend class Board;
            Alliance currentPlayerAlliance;
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
            State* state;
        public:

            explicit constexpr Builder(State& s) :
                    currentPlayerAlliance(White),
                    state(&s)
            {  }

            explicit constexpr Builder(const Board& board) :
            currentPlayerAlliance(~board.currentPlayerAlliance),
            state(board.currentState) {
                const Player* const wp = &board.whitePlayer;
                pieces[White][Pawn]    = wp->pieces[Pawn];
                pieces[White][Rook]    = wp->pieces[Rook];
                pieces[White][Knight]  = wp->pieces[Knight];
                pieces[White][Bishop]  = wp->pieces[Bishop];
                pieces[White][Queen]   = wp->pieces[Queen];
                pieces[White][King]    = wp->pieces[King];
                const Player* const bp = &board.blackPlayer;
                pieces[Black][Pawn]    = bp->pieces[Pawn];
                pieces[Black][Rook]    = bp->pieces[Rook];
                pieces[Black][Knight]  = bp->pieces[Knight];
                pieces[Black][Bishop]  = bp->pieces[Bishop];
                pieces[Black][Queen]   = bp->pieces[Queen];
                pieces[Black][King]    = bp->pieces[King];
            }

            template <Alliance A, PieceType PT>
            inline Builder& setPieces(const uint64_t p)
            { pieces[A][PT] = p; return *this; }

            template <Alliance A, PieceType PT>
            inline Builder& setPiece(const int sq)
            { pieces[A][PT] |= SquareToBitBoard[sq]; return *this; }

            template <Alliance A, PieceType PT>
            inline Builder& xorPieces(const uint64_t p)
            { pieces[A][PT] ^= p; return *this; }

            template <Alliance A, PieceType PT>
            inline Builder& andPieces(const uint64_t p)
            { pieces[A][PT] &= p; return *this; }

            template <Alliance A>
            inline Builder& removeSquare(const int sq) {
                const uint64_t p = ~SquareToBitBoard[sq];
                pieces[A][Pawn] &= p;
                pieces[A][Rook] &= p;
                pieces[A][Knight] &= p;
                pieces[A][Bishop] &= p;
                pieces[A][Rook] &= p;
                pieces[A][Queen] &= p;
                pieces[A][King] &= p;
                return *this;
            }

            inline Builder& setEnPassantSquare(const Square square)
            { state->epSquare = square; return *this; }

            template <Alliance A, CastleType CT, bool B>
            inline Builder& setCastlingRights() {
                static_assert(A == White || A == Black);
                static_assert(CT == KingSide || CT == QueenSide);
                state->setCastlingRights<A, CT, B>();
                return *this;
            }

            template <Alliance A>
            inline Builder& setCurrentPlayer()
            { currentPlayerAlliance = A; return *this; }

            [[nodiscard]]
            constexpr Board build() const { return Board(*this); }

            Builder(const Builder&) = delete;
        };
    private:

        static constexpr uint64_t
        calculateWhitePieces(const Builder& b) {
            return b.pieces[White][Pawn]   | b.pieces[White][Rook]   |
                   b.pieces[White][Knight] | b.pieces[White][Bishop] |
                   b.pieces[White][Queen]  | b.pieces[White][King];
        }

        static constexpr uint64_t
        calculateBlackPieces(const Builder& b) {
            return b.pieces[Black][Pawn]   | b.pieces[Black][Rook]   |
                   b.pieces[Black][Knight] | b.pieces[Black][Bishop] |
                   b.pieces[Black][Queen]  | b.pieces[Black][King];
        }

        constexpr void
        initMailbox(const uint64_t* const whitePieces,
                    const uint64_t* const blackPieces) {
            for (int j = Pawn; j < NullPT; ++j) {
                for (uint64_t x = whitePieces[j]; x; x &= x - 1)
                    mailbox[bitScanFwd(x)] = (PieceType) j;
                for (uint64_t x = blackPieces[j]; x; x &= x - 1)
                    mailbox[bitScanFwd(x)] = (PieceType) j;
            }
        }

        explicit constexpr Board(const Builder& b) :
                whitePlayer(Player(
                        b.pieces[White],
                        calculateWhitePieces(b),
                        true)),
                blackPlayer(Player(
                        b.pieces[Black],
                        calculateBlackPieces(b),
                        false)),
                allPieces(whitePlayer.allPieces | blackPlayer.allPieces),
                currentPlayerAlliance(b.currentPlayerAlliance),
                currentState(b.state)
        { initMailbox(b.pieces[White], b.pieces[Black]); }

        // ASSUME THAT THE MOVE IS LEGAL ! ! !
        template<Alliance A>
        constexpr void applyMove(const Move& m, State& state) {
            assert(currentState != &state);
            state.prevState = currentState;
            currentState = &state;
            const int origin      = m.origin(),
                      destination = m.destination(),
                      isPromotion = m.isPromotion();
            PieceType captureType = mailbox[destination],
                      activeType  = mailbox[origin];
            currentState->capturedPiece = captureType;
            constexpr const Alliance us = A, them = ~us;
            const uint64_t originBoard      = SquareToBitBoard[origin],
                           destinationBoard = SquareToBitBoard[destination],
                           moveBB           = originBoard | destinationBoard;
            Player* const ourPlayer   = getPlayer<us>();
            Player* const theirPlayer = getPlayer<them>();
            const Defaults* const x = getDefaults<us>();
            mailbox[origin] = NullPT;
            if(activeType == Rook) {
                if(x->kingSideRookOrigin == originBoard)
                    currentState->setCastlingRights<us, KingSide, false>();
                else if(x->queenSideRookOrigin == originBoard)
                    currentState->setCastlingRights<us, QueenSide, false>();
            } else if(activeType == King) {
                currentState->setCastlingRights<us, KingSide, false>();
                currentState->setCastlingRights<us, QueenSide, false>();
            } else {
                currentState->setCastlingRights<us, KingSide>(
                        currentState->prevState->
                                getCastlingRights<us, KingSide>()
                );
                currentState->setCastlingRights<us, QueenSide>(
                        currentState->prevState->
                                getCastlingRights<us, QueenSide>()
                );
            }
            currentPlayerAlliance = them;
            if(isPromotion) {
                ourPlayer->pieces[activeType]    ^= moveBB;
                ourPlayer->allPieces             ^= moveBB;
                theirPlayer->allPieces &= ~destinationBoard;
                theirPlayer->pieces[captureType] &= ~destinationBoard;
                allPieces = whitePlayer.allPieces | blackPlayer.allPieces;
                mailbox[destination] = PieceType(m.promotionPiece());
                return;
            }
            mailbox[destination] = activeType;
            const int moveType = m.moveType();
            if(moveType == FreeForm || moveType == PawnJump) {
                ourPlayer->pieces[activeType]    ^= moveBB;
                ourPlayer->allPieces             ^= moveBB;
                theirPlayer->allPieces &= ~destinationBoard;
                theirPlayer->pieces[captureType] &= ~destinationBoard;

                allPieces = whitePlayer.allPieces | blackPlayer.allPieces;
                currentState->epSquare =
                        moveType == PawnJump ? (Square) destination : NullSQ;
                return;
            }
            if(moveType == Castling) {
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
                ourPlayer->pieces[Rook]       ^= rookMoveBB;
                ourPlayer->pieces[activeType] ^= moveBB;
                ourPlayer->allPieces          ^= fullBB;
                allPieces                     ^= fullBB;
                currentState->setCastlingRights<us, KingSide, false>();
                currentState->setCastlingRights<us, QueenSide, false>();
            }
            else {
                const int epSquare = currentState->prevState->epSquare;
                const uint64_t captureBB = SquareToBitBoard[epSquare];
                ourPlayer->pieces[Pawn] ^= moveBB;
                ourPlayer->allPieces ^= moveBB;
                theirPlayer->pieces[Pawn] ^= captureBB;
                theirPlayer->allPieces ^= captureBB;
                allPieces = whitePlayer.allPieces | blackPlayer.allPieces;
                mailbox[epSquare] = NullPT;
            }
            currentState->epSquare = NullSQ;
        }

        // ASSUME THAT THE MOVE IS LEGAL ! ! !
        template<Alliance A>
        constexpr void retractMove(const Move& m) {
            constexpr const Alliance us = A, them = ~us;
            const int origin      = m.origin(),
                      destination = m.destination(),
                      isPromotion = m.isPromotion();
            PieceType captureType = currentState->capturedPiece,
                      activeType  = mailbox[destination];
            const uint64_t originBoard      = SquareToBitBoard[origin],
                           destinationBoard = SquareToBitBoard[destination],
                           moveBB           = originBoard | destinationBoard;
            Player* const ourPlayer     = getPlayer<us>();
            Player* const theirPlayer   = getPlayer<them>();
            const Defaults* const x = getDefaults<us>();
            currentPlayerAlliance = us;
            mailbox[origin] = activeType;
            mailbox[destination] = captureType;
            if(isPromotion) {
                ourPlayer->pieces[activeType]    ^= moveBB;
                ourPlayer->allPieces             ^= moveBB;
                if(captureType != NullPT) {
                    theirPlayer->allPieces |= destinationBoard;
                    theirPlayer->pieces[captureType] |= destinationBoard;
                }
                allPieces = whitePlayer.allPieces | blackPlayer.allPieces;
                return;
            }
            const int moveType = m.moveType();
            if(moveType == FreeForm || moveType == PawnJump) {
                ourPlayer->pieces[activeType]    ^= moveBB;
                ourPlayer->allPieces             ^= moveBB;
                if(captureType != NullPT) {
                    theirPlayer->allPieces |= destinationBoard;
                    theirPlayer->pieces[captureType] |= destinationBoard;
                }
                allPieces = whitePlayer.allPieces | blackPlayer.allPieces;
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
                ourPlayer->pieces[Rook]       ^= rookMoveBB;
                ourPlayer->pieces[activeType] ^= moveBB;
                ourPlayer->allPieces          ^= fullBB;
                allPieces                     ^= fullBB;
            }
            else {
                const uint64_t epSquare  = currentState->prevState->epSquare;
                const uint64_t captureBB = SquareToBitBoard[epSquare];
                ourPlayer->pieces[Pawn]   ^= moveBB;
                ourPlayer->allPieces      ^= moveBB;
                theirPlayer->allPieces |= captureBB;
                theirPlayer->pieces[Pawn] |= captureBB;
                allPieces = whitePlayer.allPieces | blackPlayer.allPieces;
                mailbox[epSquare] = Pawn;
            }
            currentState = currentState->prevState;
        }

    public:

        [[nodiscard]]
        constexpr const Player* getWhitePlayer() const
        { return &whitePlayer; }

        [[nodiscard]]
        constexpr const Player* getBlackPlayer() const
        { return &blackPlayer; }

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
