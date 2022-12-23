#include <stddef.h>
#include <cstdint>
#include <cstdio>
#include <stdlib.h>

/*

The playing field is 8x8, which is 64 tiles. For efficient computing,
each field corresponds to a single bit. Because each field can be either
empty, black, or white, we need at least two bits per field. This is done
by storing each player's moves in separate ints. For a valid board, it's
important that there are no overlapping moves, in other words
`(board[0] & board[1]) == 0` needs to be true at all times.

Because of the mapping, increasing the index by 1 equals moving a field
to the right, increasing by 8 is moving one row down, subtracting is the
opposite, and combining also works (+7 is down-left). Do watch out that
this movement system doesn't take field borders into account, which could
give some weird results if not accounted for properly.

*/
const int8_t directions[4] = {1, 7, 8, 9};

void print_board(uint64_t a, uint64_t b = 0) {
    if (a & b) printf("Warning, boards overlap!");
    printf("╔════════╗\n");
    for (size_t x = 0; x < 8; x++) {
        printf("║");
        for (size_t y = 0; y < 8; y++) {
            if (a & 1) printf("X");
            else if (b & 1) printf("O");
            else printf(".");
            a >>= 1;
            b >>= 1;
        }
        printf("║\n");
    }
    printf("╚════════╝\n");
}

// https://en.wikipedia.org/wiki/Hamming_weight
int popcount64c(uint64_t x) {
    const uint64_t m1 = 0x5555555555555555; //binary: 0101...
    const uint64_t m2 = 0x3333333333333333; //binary: 00110011..
    const uint64_t m4 = 0x0f0f0f0f0f0f0f0f; //binary:  4 zeros,  4 ones ...
    const uint64_t h01 = 0x0101010101010101; //the sum of 256 to the power of 0,1,2,3...
    x -= (x >> 1) & m1;             //put count of each 2 bits into those 2 bits
    x = (x & m2) + ((x >> 2) & m2); //put count of each 4 bits into those 4 bits
    x = (x + (x >> 4)) & m4;        //put count of each 8 bits into those 8 bits
    return (x * h01) >> 56;  //returns left 8 bits of x + (x<<8) + (x<<16) + (x<<24) + ...
}

/*
Get all possible moves that enclose opposite pieces, in the direction that
increases the index, so left, up, up-left, up-right. This checks in reverse,
so from end piece to empty spot, which means the internal shift is increasing.
Because it's impossible to enclose pieces to the right from the right edge,
we use the right edge as separating row.
*/
uint64_t moves_down(const uint64_t friendly, const uint64_t enemy, const uint64_t empty, const uint8_t increment) {
    uint64_t valid = 0;
    uint64_t candidates = enemy & (friendly << increment);
    // if(increment == 1) print_board(candidates);
    while (candidates != 0) {
        valid |= empty & (candidates << increment) & ~0x0101010101010101;
        candidates = enemy & (candidates << increment) & ~0x0101010101010101;
        // if(increment == 1) print_board(valid);
        // if(increment == 1) print_board(candidates);
    }
    return valid;
}

// Get all possible moves that increases the index: right, down, down-left, down-right
uint64_t moves_up(const uint64_t friendly, const uint64_t enemy, const uint64_t empty, const uint8_t increment) {
    uint64_t valid = 0;
    uint64_t candidates = enemy & (friendly >> increment);
    // if(increment == 1) print_board(candidates);
    while (candidates != 0) {
        valid |= empty & (candidates >> increment) & ~0x8080808080808080;
        candidates = enemy & (candidates >> increment) & ~0x8080808080808080;
        // if(increment == 1) print_board(valid);
        // if(increment == 1) print_board(candidates);
    }
    return valid;
}

/*
A move is only valid if it outflanks at least one piece of the opponent.
Of course, you can't place a piece on a field that's already occupied.
Because outflanking requires adjacency, we can already rule out any field
that doesn't have any adjacent marks of the opponent.

This function is hardcoded to check for the black player, but switching the
inputs is enough to check for the other player.
*/
uint64_t getValidMoves(uint64_t friendly, uint64_t enemy) {
    uint64_t valid = 0;
    uint64_t combined = friendly | enemy;
    uint64_t empty = ~combined;
    for (int8_t direction: directions) {
        valid |= moves_up(friendly, enemy, empty, direction);
        valid |= moves_down(friendly, enemy, empty, direction);
    }
    return valid;
}

uint64_t flips_up(const uint64_t friendly, const uint64_t enemy, const uint64_t empty, const uint64_t move,
                  const uint8_t increment) {
    uint64_t candidates = enemy & (move << increment);
    uint64_t flips = candidates;
    while (candidates != 0) {
        // If we hit an empty spot, it's not enclosed,
        // and thus nothing should be flipped
        if ((empty | 0x0101010101010101) & (candidates << increment)) return 0;
        // If we hit one of our pieces, it's enclosed,
        // and we should return the bits
        if (friendly & (candidates << increment)) return candidates;

        candidates = enemy & (candidates << increment);
        flips |= candidates;
    }
    return 0;
}

uint64_t flips_down(const uint64_t friendly, const uint64_t enemy, const uint64_t empty, const uint64_t move,
                    const uint8_t increment) {
    uint64_t candidates = enemy & (move >> increment);
    uint64_t flips = candidates;
    while (candidates != 0) {
        if ((empty | 0x8080808080808080) & (candidates >> increment)) return 0;
        if (friendly & (candidates >> increment)) return candidates;
        candidates = enemy & (candidates << increment);
        flips |= candidates;
    }
    return 0;
}

uint64_t get_flips(uint64_t friendly, uint64_t enemy, uint8_t move) {
    uint64_t valid = 0;
    uint64_t empty = ~(friendly | enemy);
    uint64_t move_mask = 1L << move;
    for (int8_t direction: directions) {
        valid |= flips_up(friendly, enemy, empty, move_mask, direction);
        valid |= flips_down(friendly, enemy, empty, move_mask, direction);
    }
    return valid | move_mask;
}

uint8_t pick_move(uint64_t moves) {
    int options = popcount64c(moves);
    // Doesn't generate a uniform distribution, but good enough for what we do
    int ith_bit = rand() % options;

    for (size_t i = 0; i < 64; i++) {
        if ((moves & 1) && ith_bit-- == 0) return i;
        moves >>= 1;
    }
    printf("Error, picked invalid move");
    return 0;
}

void swap(uint64_t *a, uint64_t *b) {
    uint64_t c = *a;
    *a = *b;
    *b = c;
}


struct game_state {
    uint64_t a, b;
    bool skip;
};

void swap(game_state &board) {
    uint64_t c = board.a;
    board.a = board.b;
    board.b = c;
}

void do_move(game_state *in_board, game_state *out_board, uint8_t move) {
    uint64_t flips = get_flips(in_board->a, in_board->b, move);
    out_board->b = in_board->a | flips;
    out_board->a = in_board->b & ~flips;
}

void do_move(game_state *board, uint8_t move) {
    uint64_t flips = get_flips(board->a, board->b, move);
    board->a |= flips;
    board->b &= ~flips;
    swap(&board->a, &board->b);
}
