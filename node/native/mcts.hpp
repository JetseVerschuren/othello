#include <cstdint>
#include <vector>
#include <chrono>
#include <atomic>
#include <thread>
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

    static void DetermineMoveThread(Node *base_node, const bool *stop, std::atomic<unsigned long> *combined_iterations);
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
}

void MCTS::DetermineMoveThread(Node *base_node, const bool *stop, std::atomic<unsigned long> *combined_iterations) {
    unsigned long iterations = 0;
    while(!*stop) {
        iterations++;

        Node *promising = base_node->SelectPromisingChild();
        promising->Expand();
        promising = promising->GetRandomChild();
        bool wins = promising->PlayRandomGame();
        promising->BackPropogate(wins);
    }

    *combined_iterations += iterations;
}

uint8_t MCTS::DetermineMove(unsigned int runtime) {
    auto duration = std::chrono::milliseconds(runtime);

    bool stop = false;
    std::atomic<unsigned long> combined_iterations(0);
    std::vector<std::thread> threads;
    threads.reserve(root->GetChildren()->size());

    root->Expand();
    for (auto &child: *root->GetChildren()) {
        threads.emplace_back(DetermineMoveThread, &child, &stop, &combined_iterations);
    }
//    threads.emplace_back(DetermineMoveThread, root, &stop, &combined_iterations);

    std::this_thread::sleep_for(duration);
    stop = true;

    for(auto &thread : threads) thread.join();

    printf("Did %lu iterations in %0.2f seconds, which is %.0f/s\n", combined_iterations.load(),
           static_cast<float>(duration.count()) / 1000.0,
           static_cast<float>(combined_iterations) / static_cast<float>(duration.count()) * 1000.0);
    printf("Tree size: %d\n", root->TreeSize());

    return root->GetBestMove();
}

std::vector<int8_t> MCTS::GetBoard() {
    return game.ToVector();
}

bool MCTS::OpponentCanMove() {
    return game.OpponentCanMove();
}
