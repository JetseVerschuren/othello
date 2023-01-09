#include <cstdio>
#include "mcts.hpp"

int main() {
    MCTS mcts;
    std::vector<uint8_t> moves;
    for (int i = 0; i < 10; ++i) {
        auto move = mcts.DetermineMove(2000);
        printf("Chose move %d!\n", move);
        if(move != 64 && std::count(moves.begin(), moves.end(), move)) {
            printf("Duplicate move!");
            break;
        } else {
            moves.push_back(move);
        }
        mcts.ApplyMove(move);
    }
}
