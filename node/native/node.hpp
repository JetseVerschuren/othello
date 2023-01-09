#include <climits>
#include <cmath>
#include <algorithm>

class Node {
public:
    Node(uint8_t move, Node *parent);

    Node(Node *base);

    Node *SelectPromisingChild();

    void Expand();

    Node *GetRandomChild();

    bool PlayRandomGame();

    void BackPropogate(bool won);

    Node *ApplyMove(uint8_t i);

    uint8_t GetBestMove();

    unsigned int TreeSize();

private:
    Othello game;
    unsigned int visit_count = 0;
    int win_score = 0;
    uint8_t move;

    Node *parent;
    std::vector<Node> children;

    static double UctScore(unsigned int totalVisit, double nodeWinScore, unsigned int nodeVisit);
};

Node::Node(uint8_t move, Node *parent = nullptr) {
    this->parent = parent;
    this->move = move;
}

/*
 * Move base into this new Node
 * The children are moved (vector of base is empty after this), other ints are untouched
 */
Node::Node(Node *base) {
    this->parent = nullptr;
    this->visit_count = base->visit_count;
    this->win_score = base->win_score;
    this->game = base->game;
    this->move = base->move;
    this->children.swap(base->children);
    for (auto &child: children) {
        child.parent = this;
    }
}

Node *Node::SelectPromisingChild() {
    Node *promising = this;
    while (!promising->children.empty()) {
        // TODO: Use max_element
        Node *best_child = nullptr;
        double best_score = INT_MIN;
        for (auto &child: promising->children) {
            double score = Node::UctScore(promising->visit_count, child.win_score, child.visit_count);
            if (score > best_score) {
                best_score = score;
                best_child = &child;
            }
        }
        promising = best_child;
    }
    return promising;
}

double Node::UctScore(unsigned int totalVisit, double nodeWinScore, unsigned int nodeVisit) {
    if (nodeVisit == 0) return INT_MAX;
    return ((double) nodeWinScore / (double) nodeVisit)
           + 1.41 * sqrt(log(totalVisit) / (double) nodeVisit);
}

void Node::Expand() {
    // TODO: Benchmark if it's faster to keep a flag if it's already expanded
    if (!children.empty()) {
//        printf("Expand, non empty children. %zu children present", children.size());
        return;
    }

    uint64_t moves = game.GetValidMoves();
    if (moves == 0) {
//        printf("No moves possible\n");
        // If the opponent can't move as well, this is the end of the game, otherwise skip
        if (!game.OpponentCanMove()) {
//            printf("End of game\n");
            return;
        }
        children.emplace_back(64, this);
    }

    int nr_moves = Othello::popcount64c(moves);
    children.reserve(nr_moves);


    for (int i = 0; i < 64; ++i) {
        if (moves & 1) {
            children.emplace_back(i, this);
            game.DoMove(i, &children.back().game);
        }
        moves >>= 1;
    }
//    printf("%d children initialised for node %p\n", i, this);
}

// Pick a random child, return self if there's no children
Node *Node::GetRandomChild() {
//    printf("Children length: %zu\n", children.size());
    if (children.empty()) {
        return this;
    }
    // TODO: Use more efficient random number generator
    int child = rand() % children.size();
//    printf("Selected child: %d\n", child);
    return &children[child];
}

uint8_t pick_random_move(uint64_t moves) {
    int options = Othello::popcount64c(moves);
    // Doesn't generate a uniform distribution, but good enough for what we do (I hope)
    int ith_bit = rand() % options;

    for (size_t i = 0; i < 64; i++) {
        if ((moves & 1) && ith_bit-- == 0) return i;
        moves >>= 1;
    }
//    printf("Error, picked invalid move");
    return 0;
}

bool Node::PlayRandomGame() {
//    printf("Random game\n");
    Othello tmp_game = game;
    // TODO: Test if first flag is faster
    if (tmp_game.GetValidMoves() == 0 && !tmp_game.OpponentCanMove() && !tmp_game.win()) {
//        printf("Instant loss\n");
        parent->win_score = INT_MIN;
        return false;
    }
    while (true) {
//        printf("Move loop\n");
        uint64_t moves = tmp_game.GetValidMoves();
        if (moves == 0) {
//            printf("No available moves\n");
            // TODO: There's game logic here, I don't like it
            // TODO: Test if skip flag is faster
            // If the opponent can't move, this is the end of the game
            if (!tmp_game.OpponentCanMove()) break;
            tmp_game.DoMove(64);
            continue;
        }
//        printf("Picking random number\n");
        uint8_t random_move = pick_random_move(moves);
//        printf("Move: %d\n", random_move);
        tmp_game.DoMove(random_move);
    }
    // TODO: Should this be inverted? Too tired to think about it
    return tmp_game.win();
}

void Node::BackPropogate(bool won) {
    Node *node = this;
    while (node != nullptr) {
        node->visit_count++;
        node->win_score += won;
        // Each parent is from the opponent, who lost if we won
        won = !won;
        node = node->parent;
    }
}

Node *Node::ApplyMove(uint8_t i) {
    auto child = std::find_if(children.begin(), children.end(), [i](auto &node) { return node.move == i; });
    auto new_root = new Node(&(*child));
    delete this;
    return new_root;
}

uint8_t Node::GetBestMove() {
//    for(auto &child : children) printf("Move: %d\tVisit count: %d\n", child.move, child.visit_count);
    auto iter = std::max_element(children.begin(), children.end(), [](auto &a, auto &b){return a.visit_count < b.visit_count;});
    return iter->move;
}

unsigned int Node::TreeSize() {
    unsigned int size = children.size();
    for(auto &child : children) size += child.TreeSize();
    return size;
}
