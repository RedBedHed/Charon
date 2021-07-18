#include "ChaosMagic.h"
#include "MoveMake.h"
#ifdef _WIN32
#include <Windows.h>
#else
#include <unistd.h>
#endif
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
    if(depth <= 1) return j;
    for(MoveWrap* n = m; n->move.getManifest() != 0; ++n) {
        State x{};
        //cout << depth  << '\n';
        //if(depth == 2) cout << "before: \n" << *b << "\n\n";
            b->applyMove(n->move, x);
            i += perft(b, depth - 1);
            b->undoMove(n->move);
    }
    return i;
}

int main(int argc, char** argv) {
    Witchcraft::init();
    State s{NullSQ, nullptr, NullPT, CastlingRights(), CastlingRights()};
    Board b = Board::Builder(s).build();

    cout << "\n\t<<*. Performance Test .*>>" << '\n';
    cout << "\n\tStarting Position:\n" << b << '\n';
    int n = argv[1][0] - 48;
    for (int i = 0; i <= n; ++i) {
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


