#include "ChaosMagic.h"
#include "MoveMake.h"
#ifdef _WIN32
#include <Windows.h>
#else
#include <unistd.h>
#endif
#include <chrono>
#include <iostream>
#include <fstream>

using std::chrono::steady_clock;
using std::chrono::nanoseconds;
using std::chrono::microseconds;
using std::chrono::milliseconds;
using std::chrono::duration_cast;
using std::cout;
using namespace Charon;

uint64_t perft(Board* const b, int depth) {
    Move m[256];
    uint64_t i = 0, j;

    if(board->getPieces(board->getCurrentPlayer(), King) == 0) return 0;
    if(depth <= 1) {
        j = MoveFactory::generateMoves<All>(b, m);
        //for(Move* n = m; n->getManifest() != 0; ++n)
        //    if(n->moveType() == Castling) i++;
        return j;
    }
    j = MoveFactory::generateMoves<All>(b, m);
    for(Move* n = m; n->getManifest() != 0; ++n) {
        State x;
        b->applyMove(*n, x);
        i += perft(b, depth - 1);
        b->undoMove(*n);
    }
    return i;

}

int main(int argc, char** argv) {
    Witchcraft::init();
    State x;
    Board b = Board::Builder(x).build();
    cout << "\n\t<<*. Performance Test .*>>" << '\n';
    cout << "\n\tStarting Position:\n" << b << '\n';
    int n = argv[1][0] - 48;
    for (int i = 1; i <= n; ++i) {
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


