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
               (checkBoard & (checkBoard - 1)) > 0?
               DoubleCheck: Check;
    }

    namespace MoveFactory {
        template<FilterType FT>
        void generateMoves(Board *, MoveWrap *);
    }
}


#endif //CHARON_MOVEMAKE_H
