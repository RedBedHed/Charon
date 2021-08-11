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

int displayUsage();
int charPerft(int, int, const char**);
int charVerify(int, int, const char**);
uint64_t perft(Board*, int);

int main(const int argc, const char** const argv) {
    if(argc <= 2 ||
       argv[1][0] != '-' ||
       (argv[1][1] != 'v' &&
       argv[1][1] != 'p'))
        return displayUsage();
    int n = atoi(argv[2]);
    if(n <= 0) displayUsage();
    return argv[1][1] == 'p'? charPerft(n, argc, argv) :
           argv[1][1] == 'v'? charVerify(n, argc, argv):
           displayUsage();
}

inline int charPerft(const int n, const int argc, const char** const argv) {
    Witchcraft::init();
    State x;
    Board b = (argc == 3) ?
              Board::Builder<Default>(x).build() :
              FenUtility::parseBoard(argv[3], &x);
    cout << '\n'
         << "     @@@    @@\n"
         << "   @@   @@  @@\n"
         << "  @@        @@ @@@      @@@@    @@ @@@      @@@@    @@ @@@\n"
         << "  @@        @@@   @@  @@   @@@  @@@   @@  @@    @@  @@@   @@\n"
         << "  @@        @@    @@  @@    @@  @@        @@    @@  @@    @@\n"
         << "   @@   @@  @@    @@  @@   @@@  @@        @@    @@  @@    @@\n"
         << "     @@@    @@    @@   @@@@ @@  @@          @@@@    @@    @@\n\n"
         << "~^*^~._.~^*^~._.~^*^~._.~^*^~._.~^*^~._.~^*^~._.~^*^~._.~^*^~.\n\n"
         << "\n\t.~* Charon Perft *~.\n"
         << "\n\t*. by Ellie Moore .*\n"
         << "\n\tStarting Position:\n" << b << '\n';
    uint64_t j;
    for (int i = 1; i <= n; ++i) {
        double start = clock();
        j = perft(&b, i);
        double stop = clock() - start;
        cout << "\n\tperft(" << i << ") - ";
        printf("%6.3f", (double) stop / (double) CLOCKS_PER_SEC);
        cout << " seconds - ";
        printf("%10llu", j);
        cout << " nodes visited.";
    }
    cout << "\n\n\tEnding Position:\n" << b << '\n';
    cout << "~^*^~._.~^*^~._.~^*^~._.~^*^~._.~^*^~._.~^*^~._.~^*^~._.~^*^~.\n\n";
    Witchcraft::destroy();
    return 0;
}

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

inline int charVerify(const int n, const int argc, const char** const argv) {
    Witchcraft::init();
    State x;
    if(argc == 3) return displayUsage();
    const int q = atoi(argv[4]),
              z = atoi(argv[5]);
    if(q <= 0) return displayUsage();
    Board b = FenUtility::parseBoard(argv[3], &x);
    uint64_t j;
    for (int i = 1; i <= n; ++i) j = perft(&b, i);
    cout << (z? (int) z: (char)'-')      << ' '
         << (j == q? "passed": "failed") << '\n';
    Witchcraft::destroy();
    return 0;
}

inline int displayUsage() {
    cout << "Usage: ./cc0 [\"-p\"|\"-v\"] [depth] {FEN} {count} <number>\n\n"
         << "Usage Symbols (do not pass these with args)\n"
         << "[] : required argument\n"
         << "\"\": literal\n"
         << "<> : optional argument\n"
         << "{} : if -v, pass this argument, else this argument is optional\n\n"
         << "Key\n"
         << "-p     : normal q-perft style perft mode\n"
         << "-v     : verification mode (for shell script use)\n"
         << "depth  : the perft depth (a positive integer)\n"
         << "FEN    : a board in Forsyth-Edwards Notation\n"
         << "count  : the node count to verify\n"
         << "number : an integer to represent the line of the client script\n";
    return 0;
}


