//
// Created by evcmo on 6/6/2021.
//

#pragma once
#ifndef CHARON_MOVE_H
#define CHARON_MOVE_H

#include <cstdint>
#include <cassert>
#include <iostream>
#include <ostream>
#include <string>

namespace Charon {

    /** The alliances, enumerated. */
    enum Alliance : uint8_t
    { White, Black };

    constexpr Alliance operator~(const Alliance& a)
    { return a == White? Black : White; }

    enum PieceType : uint8_t
    { Pawn, Rook, Knight, Bishop, Queen, King, NullPT };

    enum MoveType : uint8_t
    { FreeForm, EnPassant, Castling, PawnJump };

    enum FilterType : uint8_t
    { Aggressive, Defend, Passive, All };

    constexpr const char* MoveTypeToString[] =
    { "FreeForm", "EnPassant", "Castling", "PawnJump" };

    constexpr const char* PieceTypeToString[] =
    { "Pawn", "Rook", "Knight", "Bishop", "Queen", "King", "NullPT" };

    class Move final {
    private:

        static constexpr uint16_t To = 0x03FU;
        static constexpr uint16_t From = 0xFC0U;
        static constexpr uint16_t Type = 0x3000U;
        static constexpr uint16_t Promotion = 0x8000U;
        static constexpr uint16_t PromoPiece = 0x3000U;

        uint16_t manifest;

    public:

        explicit constexpr Move(const unsigned int manifest) :
            manifest(manifest)
        { }

        explicit constexpr Move() : manifest(0) {}

        Move(const Move& move) = default;
        ~Move() = default;

        [[nodiscard]]
        constexpr int getManifest() const
        { return manifest; }

        static constexpr Move make(const unsigned int manifest)
        { return Move(manifest); }

        template<MoveType MT>
        static constexpr Move make(unsigned int from, unsigned int to)
        { return Move((MT << 12U) + (from << 6U) + to); }

        static constexpr Move
        make(const unsigned int from, const unsigned int to)
        { return Move(((from << 6U) + to)); }

        template<PieceType PT>
        static constexpr Move
        makePromotion(const unsigned int from, const unsigned int to)
        { return Move((Promotion << 15U) + (PT << 12U) + ((from << 6U) + to)); }

        [[nodiscard]]
        constexpr int destination() const
        { return manifest & To; }

        [[nodiscard]]
        constexpr int origin() const
        { return (manifest & From) >> 6U; }

        [[nodiscard]]
        constexpr int moveType() const
        { return (manifest & Type) >> 12U; }

        [[nodiscard]]
        constexpr int isPromotion() const
        { return (manifest & Promotion) >> 15U; }

        [[nodiscard]]
        constexpr int promotionPiece() const
        { return (manifest & PromoPiece) >> 12U; }

        constexpr bool operator==(const Move& other) const
        { return manifest == other.manifest; }

        constexpr bool operator!=(const Move& other) const
        { return manifest != other.manifest; }

        constexpr Move& operator=(const Move& mx) = default;

        friend std::ostream& operator <<(std::ostream& out,
                                         const Move& m) {
            if(m.isPromotion())
                out <<"Promotion - "
                    << PieceTypeToString[m.promotionPiece()];
            else
                out << MoveTypeToString[m.moveType()];
            return out << " - From: "
                       << m.origin() << " To: "
                       << m.destination();
        }

    };

    constexpr Move NullMove = Move::make(0U);
}

#endif //CHARON_MOVE_H