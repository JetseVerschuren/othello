#include <cstdint>
#include <vector>
#include <chrono>
#include "othello.hpp"
#include "node.hpp"

class MCTS {
public:
    MCTS();

    ~MCTS();

    void ApplyMove(uint8_t move);

    uint8_t DetermineMove(unsigned int runtime);

    std::vector<int8_t> GetBoard();

    bool OpponentCanMove();

    Node *root;
    Othello game;
};

MCTS::MCTS() {
    root = new Node(0, nullptr);
}

MCTS::~MCTS() {
    delete root;
}

void MCTS::ApplyMove(uint8_t move) {
    game.DoMove(move);
    root->Expand();
    root = root->ApplyMove(move);
    printf("Tree size: %d\n", root->TreeSize());
}

uint8_t MCTS::DetermineMove(unsigned int runtime) {
    auto duration = std::chrono::milliseconds(runtime);
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
    printf("Did %lu iterations in %0.2f seconds, which is %.0f/s\n", iterations,
           static_cast<float>(duration.count()) / 1000.0,
           static_cast<float>(iterations) / static_cast<float>(duration.count()) * 1000.0);
    printf("Tree size: %d\n", root->TreeSize());

    return root->GetBestMove();
}

std::vector<int8_t> MCTS::GetBoard() {
    return game.ToVector();
}

bool MCTS::OpponentCanMove() {
    return game.OpponentCanMove();
}
