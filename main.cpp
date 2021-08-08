#include "ChaosMagic.h"
#include "MoveMake.h"
#include "Fen.h"
#ifdef WIN32
#include <Windows.h>
#else
#include <unistd.h>
#endif
#include <time.h>
#include <iostream>
#include <fstream>
#include <sstream>

using std::cout;
using std::flush;
using namespace Charon;

uint64_t perft(Board* const b, int depth) {
    Move m[256];
    uint64_t i = 0, j;
    j = MoveFactory::generateMoves<All>(b, m);
    if(depth <= 1) return j;
    for(Move* n = m; n->getManifest() != 0; ++n) {
        State x;
        if (b->getPiece(n->destination()) != King) {
            b->applyMove(*n, x);
            i += perft(b, depth - 1);
            b->retractMove(*n);
        }
    }
    return i;
}

int main(int argc, const char** argv) {
    if(argc <= 1) {
        cout << "Usage: ./exe [integer depth] <FEN string>\n";
        return 0;
    }
    Witchcraft::init();
    State x;
    Board b = (argc == 2) ?
            Board::Builder<Default>(x).build() :
            FenUtility::parseBoard(argv[2], &x);
    cout << "\n\t.~* Charon Perft *~." << '\n';
    cout << "\n\t*.    by Ellie    .*" << '\n';
    cout << "\n\tStarting Position:\n" << b << '\n';
    int n = atoi(argv[1]);
    for (int i = 1; i <= n; ++i) {
        double start = clock();
        uint64_t j = perft(&b, i);
        double stop = clock() - start;
        cout << "\n\tperft(" << i << ") - ";
        printf("%6.3f", (double) stop / (double) CLOCKS_PER_SEC);
        cout << " seconds - ";
        printf("%10llu", j);
        cout << " nodes visited.";
    }
    cout << "\n\n\tEnding Position:\n" << b << '\n';
    Witchcraft::destroy();
}


