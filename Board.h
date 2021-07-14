//
// Created by evcmo on 6/8/2021.
//

#ifndef CHARON_BOARD_H
#define CHARON_BOARD_H
#include <iostream>
#include <cstdint>
#include <locale>
#include "ChaosMagic.h"

namespace Charon {

    using namespace Witchcraft;

    enum CastleType : uint8_t
    { KingSide, QueenSide };

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
        const uint64_t  kingSideMask;
        const uint64_t  queenSideMask;
        const uint64_t  kingSideDestination;
        const uint64_t  queenSideDestination;
    };

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
            WhiteOoMask,
            WhiteOooMask,
            WhiteKingsideKingDestination,
            WhiteQueensideKingDestination
    };

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
            BlackKingsideMask,
            BlackQueensideMask,
            BlackKingsideKingDestination,
            BlackQueensideKingDestination
    };

    template <Alliance A>
    constexpr const Defaults* getDefaults()
    { return A == White ? &WhiteDefaults : &BlackDefaults; }

    struct CastlingRights final {
    public:
        bool queenSide;
        bool kingSide;

        template <CastleType CT>
        constexpr bool get()
        { return CT == KingSide ? kingSide : queenSide; }

        template <CastleType CT>
        constexpr void set(const bool rights)
        { if(CT == KingSide) kingSide = rights;
            else queenSide = rights; }
    };

    class Board;

    class Player final {
    private:

        friend class Position;
        friend class Board;

        const Board* const board;
        uint64_t pieces[7]{};
        uint64_t allPieces;
        uint64_t white;
        CastlingRights castlingRights;
    public:

        constexpr Player(const Board* board,
                         const uint64_t* const p,
                         const uint64_t allPieces,
                         const bool isWhite,
                         const CastlingRights castlingRights) :
                board(board), allPieces(allPieces),
                white(isWhite), castlingRights(castlingRights) {
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

        template <CastleType CT>
        [[nodiscard]]
        constexpr bool hasCastlingRights() const {
            return CT == KingSide?
                castlingRights.kingSide :
                castlingRights.queenSide;
        }
    };

    class Board final {
    private:

        friend class Player;

        Player whitePlayer;
        Player blackPlayer;
        short epSquare;
        uint64_t allPieces;
        Alliance currentPlayerAlliance;
        PieceType board[BoardLength] {
            NullPT, NullPT, NullPT, NullPT, NullPT, NullPT, NullPT, NullPT,
            NullPT, NullPT, NullPT, NullPT, NullPT, NullPT, NullPT, NullPT,
            NullPT, NullPT, NullPT, NullPT, NullPT, NullPT, NullPT, NullPT,
            NullPT, NullPT, NullPT, NullPT, NullPT, NullPT, NullPT, NullPT,
            NullPT, NullPT, NullPT, NullPT, NullPT, NullPT, NullPT, NullPT,
            NullPT, NullPT, NullPT, NullPT, NullPT, NullPT, NullPT, NullPT,
            NullPT, NullPT, NullPT, NullPT, NullPT, NullPT, NullPT, NullPT,
            NullPT, NullPT, NullPT, NullPT, NullPT, NullPT, NullPT, NullPT
        };

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
            uint8_t epSquare;
            Alliance currentPlayerAlliance;
            CastlingRights whiteCastlingRights;
            CastlingRights blackCastlingRights;
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
        public:

            explicit constexpr Builder() :
                    epSquare(NullSQ),
                    currentPlayerAlliance(White),
                    whiteCastlingRights(CastlingRights {
                        true, true
                    }),
                    blackCastlingRights(CastlingRights {
                            true, true
                    })
            {  }

            explicit constexpr Builder(const Board& board) :
            epSquare(board.epSquare),
            currentPlayerAlliance(~board.currentPlayerAlliance),
            whiteCastlingRights(board.whitePlayer.castlingRights),
            blackCastlingRights(board.blackPlayer.castlingRights) {
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
            inline Builder& applyMoveBoard(const uint64_t p)
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

            inline Builder& setEnPassantSquare(const short int square)
            { epSquare = square; return *this; }

            template <Alliance A, CastleType CT>
            inline Builder& setCastlingRights(const bool canCastle) {
                static_assert(A == White || A == Black);
                static_assert(CT == KingSide || CT == QueenSide);
                if(A == White)
                    whiteCastlingRights.set<CT>(canCastle);
                else
                    blackCastlingRights.set<CT>(canCastle);
                return *this;
            }

            inline Builder& setCurrentPlayer(const Alliance alliance)
            { currentPlayerAlliance = alliance; return *this; }

            [[nodiscard]]
            constexpr Board build() const { return Board(*this); }

            Builder(const Builder&) = default;
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
        initBoard(const uint64_t* const whitePieces,
                  const uint64_t* const blackPieces) {
            for (int j = Pawn; j < NullPT; ++j) {
                for (uint64_t x = whitePieces[j]; x; x &= x - 1)
                    board[bitScanFwd(x)] = (PieceType) j;
                for (uint64_t x = blackPieces[j]; x; x &= x - 1)
                    board[bitScanFwd(x)] = (PieceType) j;
            }
        }

        explicit constexpr Board(const Builder& b) :
                whitePlayer(Player(
                        this,
                        b.pieces[White],
                        calculateWhitePieces(b),
                        true,
                        b.whiteCastlingRights)),
                blackPlayer(Player(
                        this,
                        b.pieces[Black],
                        calculateBlackPieces(b),
                        false,
                        b.whiteCastlingRights)),
                epSquare(b.epSquare),
                allPieces(whitePlayer.allPieces | blackPlayer.allPieces),
                currentPlayerAlliance(b.currentPlayerAlliance)
        { initBoard(b.pieces[White], b.pieces[Black]); }

        template<Alliance A>
        constexpr void applyMove(const Move& m) {
            /*constexpr const Alliance us = A, them = ~us;
            int origin   = m.origin()  , destination = m.destination(),
                    moveType = m.moveType();
            const uint64_t originBoard = SquareToBitBoard[origin],
                           destinationBoard = SquareToBitBoard[destination];
            uint64_t moveBB  =
                    originBoard | destinationBoard;
            Player* const ourPlayer = getPlayer<us>();
            uint64_t* const theirPieces = getPlayer<them>()->pieces;
            PieceType captureType = board[destination],
                    activeType  = board[origin];
            const Defaults* const x = getDefaults<us>();

            board[origin] = NullPT;
            switch (moveType) {
                case EnPassant:
                    break;
                case Castling:
                    if(!ourPlayer->hasCastlingRights()) return;
                    if(x->kingSideMask & destinationBoard) {
                        moveBB |=
                    } else {

                    }
                    break;
                default:
                    ourPlayer->pieces[activeType] ^= moveBB;
                    ourPlayer->allPieces          ^= moveBB;
                    allPieces = whitePlayer.allPieces | blackPlayer.allPieces;
                    theirPieces[captureType] ^= destination;
                    if(moveType == PawnJump)
                        epSquare = (short) destination;
            }
            currentPlayerAlliance = them;
            board[destination] = m.isPromotion()?
                                 activeType : PieceType(m.promotionPiece());*/
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
        { return epSquare; }

        void applyMove(const Move& m) {
            if(currentPlayerAlliance == White)
                applyMove<White>(m);
            else applyMove<Black>(m);
        }
    };

}

#endif //CHARON_BOARD_H
