#include "ChaosMagic.h"
#include "MoveMake.h"
#include <time.h>
#include <iostream>
using std::cout;
using namespace Charon;

uint64_t perft(Board* const b, int depth) {
    Move m[256];
    uint64_t i = 0, j;
    j = MoveFactory::generateMoves<All>(b, m);
    if(depth <= 1) return j;
    for(Move* n = m; n->getManifest() != 0; ++n) {
        State x;
        b->applyMove(*n, x);
        i += perft(b, depth - 1);
        b->retractMove(*n);
    }
    return i;
}

int main(int argc, const char** argv) {
    Witchcraft::init();
    State x; Board b = Board::Builder(x).build();
    cout << "\n\t<<*. Performance Test .*>>" << '\n';
    cout << "\n\tStarting Position:\n" << b << '\n';
    int n = atoi(argv[1]);
    for (int i = 1; i <= n; ++i) {
        double start = clock();
        uint64_t j = perft(&b, i);
        double stop = clock() - start;
        cout << "\n\t(" << i << ") ";
        printf("%6.3f", (double) stop / (double) CLOCKS_PER_SEC);
        cout << " seconds - " << j << " nodes visited.";
    }
    cout << "\n\n\tEnding Position:\n" << b << '\n';
    Witchcraft::destroy();
}


