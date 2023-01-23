#include <cstdio>
#include "othello.hpp"

int main() {
    Othello game;

    game.DoMove(19);
    game.DoMove(18);
    game.DoMove(17);
    game.DoMove(20);
    game.DoMove(21);
    game.DoMove(12);
    game.DoMove(3);
//    game.DoMove(42);

    game.PrintBoard();
    printf("%lx\n", (game.GetValidMoves() >> (5*8)) & 0xff);
//    printf("%lx\n", game.GetValidMoves());
}
