#include "board.hpp"
#include <climits>
#include <cmath>
#include <cstring>

// How many games to play per suggestion
// Will eventually be changes to a time limit
#define ITERATIONS 100000

struct node {
    // Game state
    game_state board;
    unsigned int visit_count;
    int win_score;
    uint8_t move;

    node *parent;
    uint8_t nr_children;
    node *children;
};

node *select_promising_node(node *root) {
    while (root->nr_children != 0) {
        node *best_child = nullptr;
        double best_score = INT_MIN;
        for (int i = 0; i < root->nr_children; ++i) {
            node *current_child = &root->children[i];
            double score;
            if (current_child->nr_children == 0) score = INT_MAX;
            else {
                score = ((double) current_child->win_score / (double) current_child->visit_count) +
                        1.41 * sqrt(log(root->visit_count) / (double) current_child->visit_count);
            }
            if (score > best_score) {
                best_score = score;
                best_child = current_child;
            }
        }
        root = best_child;
    }
    return root;
}

node *get_random_child(node *root) {
    return root;
}

void expand_node(node *root) {
    // Get all possible moves from here, skipping also counts as a move
    uint64_t moves = getValidMoves(root->board.a, root->board.b);
    int nr_moves = popcount64c(moves);
    if (moves == 0) {
        // If the opposing player has a possible next move, we can skip, otherwise there's nothing to do
        if (getValidMoves(root->board.b, root->board.a)) nr_moves = 1;
        else return;
    }
    node *children = static_cast<node *>(calloc(sizeof(node), nr_moves));

    root->children = children;
    root->nr_children = nr_moves;

    // Skip move, otherwise we'd already have returned
    if (moves == 0) {
        node *child = &children[0];
        child->parent = root;
        child->board = root->board;
        children->board.skip = true;
        return;
    }

    uint8_t i = 0;
    for (int move = 0; move < 64; ++move) {
        if (moves & 1) {
            node *child = &children[i++];
            child->parent = root;
            do_move(&root->board, &child->board, move);
            child->move = move;
//            printf("%d\n", move);
        }
        moves >>= 1;
    }
}

// Returns true if the main player won
bool play_random_game(game_state *state) {
    // TODO: Baeldung has a check that sets the parent's winscore to MIN_VALUE
    game_state board = *state;
    while (true) {
        uint64_t moves = getValidMoves(board.a, board.b);
        if (moves == 0) {
            // If we already skipped last move, the game is over
            if (board.skip) break;
            // If this is the first skip, flag that to the next round, and make sure to switch lead player
            board.skip = true;
            swap(board);
            continue;
        } else board.skip = false;
        uint8_t move = pick_move(moves);
        do_move(&board, move);
    }
    // When the game's over, the winner is the one with the most pieces
    return popcount64c(board.a) > popcount64c(board.b);
}

void back_propagation(node *leaf, bool won) {
    while (leaf != nullptr) {
//        printf("leaf: %p\tvisit score: %d\n", leaf, leaf->visit_count);
        leaf->visit_count++;
        leaf->win_score += won;
        // The parent node is the opposing team, so invert the winning result
        won = !won;
        leaf = leaf->parent;
    }
//    printf("\n");
}

node *get_best_child(node *root) {
//    printf("root children: %d\n", root->nr_children);
    node *best_child = nullptr;
    unsigned int best_score = 0;
    for (int i = 0; i < root->nr_children; ++i) {
        node *current_child = &root->children[i];
        if (current_child->visit_count > best_score) {
            best_score = current_child->visit_count;
            best_child = current_child;
        }
    }
    return best_child;
}

node *find_best_move(node *root) {
//    printf("root: %p\n", root);
    for (size_t i = 0; i < ITERATIONS; i++) {
        node *promising = select_promising_node(root);

        expand_node(promising);
        if (promising->nr_children > 0) promising = get_random_child(promising);
        bool won = play_random_game(&promising->board);
        back_propagation(promising, won);
    }

    return get_best_child(root);
}

void free_node(node *to_free) {

}

int main() {
    node root = {
            .board = {
                    .a = 0x0000000810000000,
                    .b = 0x0000001008000000,
                    .skip = false,
            },
            .visit_count = 0,
            .win_score = 0,
            .move = 0,
            .parent = nullptr,
            .nr_children = 0,
            .children = nullptr,
    };
    node *new_node = find_best_move(&root);
    printf("iterations: %d\ttotal wins: %d\n", root.visit_count, root.win_score);
    printf("move: %d\tvisit count: %d\t win score: %d\n", new_node->move, new_node->visit_count, new_node->win_score);
    print_board(new_node->board.a, new_node->board.b);
//    print_board(root.board.a | root.board.b, 1L << new_node->move);
    return 0;
}
