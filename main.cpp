#include "ChaosMagic.h"
#include "MoveMake.h"
#include <chrono>
#include <iostream>

using std::chrono::high_resolution_clock;
using std::chrono::microseconds;
using std::chrono::milliseconds;
using std::chrono::duration_cast;
using std::cout;
using namespace Charon;

int main() {
    Witchcraft::init();
    Board b = Board::Builder()
            .setEnPassantSquare(E5)
            .setPiece<Black, Pawn>(E5)
            .setPiece<White, Pawn>(D5)
            .setPieces<White, King>(0x0000000001000000L)
            .setPiece<Black, Queen>(F5)
            .setCastlingRights<White, KingSide>(true)
            .build();


    //b.applyMove(Move::makeMinor(8, 16));
    //b.applyMove(Move::makeMinor(55, 47));
    cout << b << '\n';
    auto start = high_resolution_clock::now();
    MoveWrap m[256];
    MoveFactory::generateMoves<All>(&b, m);
    auto stop = high_resolution_clock::now();
    auto duration = duration_cast<microseconds>(stop - start);
    for(MoveWrap* n = m; n->move.getManifest() != 0;)
        cout << n++->move << '\n';
    b.applyMove(m[12].move);
    cout << '\n' << duration.count();
    //b.applyMove(m[8].move);
    cout << b << '\n';
    Witchcraft::destroy();
}
