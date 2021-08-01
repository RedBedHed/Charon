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

    namespace MoveFactory {
        template<FilterType FT>
        int generateMoves(Board*, Move*);
    }
}


#endif //CHARON_MOVEMAKE_H
