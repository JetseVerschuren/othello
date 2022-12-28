#include <cstdint>
#include <vector>
#include <chrono>
#include "othello.hpp"
#include "node.hpp"

class MCTS {
public:
    MCTS();

    void ApplyMove(uint8_t move);

    uint8_t DetermineMove();

    std::vector<int8_t> GetBoard();

    bool OpponentCanMove();

    Node *root;
    Othello game;
};

MCTS::MCTS() {
    root = new Node(0, nullptr);
}

void MCTS::ApplyMove(uint8_t move) {
    game.DoMove(move);
}

uint8_t MCTS::DetermineMove() {
    auto duration = std::chrono::seconds(10);
    auto start = std::chrono::steady_clock::now();
    unsigned long iterations = 0;
    while (std::chrono::steady_clock::now() - start < duration) {
        iterations++;

        Node *promising = root->SelectPromisingChild();
//            printf("promising: %p\n", promising);
        promising->Expand();
//            printf("expanded\n");
        promising = promising->GetRandomChild();
//            printf("promising (child): %p\n", promising);
        bool won = promising->PlayRandomGame();
//            printf("won: %b\n", won);
        promising->BackPropogate(won);
//            printf("propagated\n\n");
    }
    printf("Did %lu iterations in %ld seconds, which is %.0f/s\n", iterations, duration.count(), static_cast<float>(iterations) / static_cast<float>(duration.count()));

    // TODO: Return most promising node
    return 65;
}

std::vector<int8_t> MCTS::GetBoard() {
    return game.ToVector();
}

bool MCTS::OpponentCanMove() {
    return game.OpponentCanMove();
}