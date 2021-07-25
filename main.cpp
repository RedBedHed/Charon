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

struct MoveDetails final {
    uint64_t epMoves = 0;
    uint64_t castles = 0;
    uint64_t attack = 0;
    uint64_t all = 0;
};

/*uint64_t perft(Board* const b, int depth) {
    MoveWrap m[256];
    uint64_t i = 0;
    MoveFactory::generateMoves<All>(b, m);
    if(depth <= 1) {
        for(MoveWrap* n = m; n->move.getManifest() != 0; ++n) {
            i += b->getPiece(n->move.destination()) != NullPT? 1:0;
        }
        return i;
    }
    for(MoveWrap* n = m; n->move.getManifest() != 0; ++n) {

        //std::cout << MoveTypeToString[n->move.moveType()] << '\n';
        State x;
        const int f = depth == 2 && attacksOn<White, King>(b, bitScanFwd(b->getPlayer<White>()->getPieces<King>())) && b->getPiece(n->move.origin()) == Knight;

        if(f) {
            sleep(1);
            cout << "before\n" << *b << '\n' << "\033[H" << std::flush;
        }
        b->applyMove(n->move, x);

        if(f) {
            sleep(1);
            cout << "after\n" << *b << '\n' << "\033[H" << std::flush;
        }
        i += perft(b, depth - 1);
        b->undoMove(n->move);
    }
    return i;

}*/

uint64_t perft(Board* const b, int depth) {
    MoveWrap m[256];
    uint64_t i = 0, j;
    j = MoveFactory::generateMoves<All>(b, m);
    if(depth <= 1) {
        /*for(MoveWrap* n = m; n->move.getManifest() != 0; ++n) {

                State x;

                bool isB = b->currentPlayer() == Black;

                uint64_t cb = 0;
            if((isB && (cb = attacksOn<Black, King>(b, bitScanFwd(b->getPlayer<Black>()->getPieces<King>())))) ||
               (!isB && (cb = attacksOn<White, King>(b, bitScanFwd(b->getPlayer<White>()->getPieces<King>()))))) {

                if(b->getPiece(n->move.origin()) == Pawn) {
                    // Calculate the check type for our king.
                    sleep(1);
                    cout << "before\n" << *b << "\033[H" << std::flush;
                    b->applyMove(n->move, x);
                    sleep(1);
                    cout << "after\n" << *b << "\033[H" << std::flush;
                    b->undoMove(n->move);
                }
            }
        }*/
        /*for(MoveWrap* n = m; n->move.getManifest() != 0; ++n) {

            State x;
                if(b->getPiece(n->move.origin()) == Pawn) {
                    sleep(1);
                    // Calculate the check type for our king.
                    cout << "before\n" << *b << "\033[H" << std::flush;
                    sleep(1);
                    b->applyMove(n->move, x);
                    sleep(1);
                    cout << "after\n" << *b << "\033[H" << std::flush;
                    b->undoMove(n->move);
                }
        }*/
        return j;
    }
    for(MoveWrap* n = m; n->move.getManifest() != 0; ++n) {

        //std::cout << MoveTypeToString[n->move.moveType()] << '\n';
        State x;
        b->applyMove(n->move, x);
        i += perft(b, depth - 1);
        b->undoMove(n->move);
    }
    return i;

}

int main(int argc, char** argv) {
    Witchcraft::init();

    State x;
    MoveDetails md;
    Board b = Board::Builder(x).build();

    /*for(int i = 0; i < 64; i++) {
        for(int j = 0; j < 64; j++) {
            bb(Rays[i][j]);
            cout << '\n';
        }
    }*/
    cout << "\n\t<<*. Performance Test .*>>" << '\n';
    cout << "\n\tStarting Position:\n" << b << '\n';
    int n = argv[1][0] - 48;
    for (int i = 1; i <= n; ++i) {
        MoveWrap mw;
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


