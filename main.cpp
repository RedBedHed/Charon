#include "ChaosMagic.h"
#include "MoveMake.h"
#include <chrono>
#include <iostream>

using std::chrono::steady_clock;
using std::chrono::nanoseconds;
using std::chrono::microseconds;
using std::chrono::milliseconds;
using std::chrono::duration_cast;
using std::cout;
using namespace Charon;

uint64_t perft(Board* b, int depth) {

    MoveWrap m[256];
    uint64_t i = 0,
    j = MoveFactory::generateMoves<All>(b, m);
    if(depth == 1) return j;
    for(MoveWrap* n = m; n->move != NullMove; n++) {
        State x{};
        b->applyMove(n->move, x);
        i += perft(b, depth - 1);
        b->undoMove(n->move);
    }
    return i;
}

int main() {
    Witchcraft::init();
    State s{NullSQ, nullptr, NullPT, CastlingRights {true, true}, CastlingRights {true, true}};
    Board b = Board::Builder(s).build();

    cout << "\n\t<<*. Performance Test .*>>" << '\n';
    cout << "\n\tStarting Position:\n" << b << '\n';
    for (int i = 1; i <= 6; i++) {
        auto start = steady_clock::now();
        uint64_t j = perft(&b, i);
        auto stop = steady_clock::now();
        auto duration = duration_cast<nanoseconds>(stop - start);
        cout << "\n\t(" << i << ") "
             << (double) duration.count() / 1000000000.0
             << " seconds - " << j << " nodes visited.";
    }
    cout << "\n\n\tEnding Position:\n" << b << '\n';
    Witchcraft::destroy();
}


