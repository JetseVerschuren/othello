#include "board.hpp"

int main() {
    // uint64_t total = 0;
    // for (size_t i = 0; i < 115000000; i++) {
    //     total += getValidMoves(i+0x0000000810000000, 0x0000001008000000);
    // }
    // printf("%ld\n", total);

    // if(getValidMoves(42, 69) != 128) printf("Test case 1 failed\n");
    // if(getValidMoves(69, 42) != 16) printf("Test case 2 failed\n");
    // if(getValidMoves(0x0000000810000000, 0x0000001008000000) != 0x0000102004080000) printf("Start position\n");
    // if(getValidMoves(0x0000000000002000, 0x000000000000C000) != 0x0000000000000000) printf("Up wrap test failed\n");
    // if(getValidMoves(0x0000000000000400, 0x0000000000000300) != 0x0000000000000000) printf("Down wrap test failed\n");
    // if(getValidMoves(0x0000000008000000, 0x00000000040C0000) != 0x0000000002000A00) printf("Down test failed\n");
    // if(getValidMoves(0x0000000008000000, 0x0000001810000000) != 0x0000280020000000) printf("Up test failed\n");
    //
    // // print_board(0x0000800400000000, 0x0000004028000000);
    // if(get_flips(0x0000800400000000, 0x0000004028000000, 20) != 0x0000004028000000) printf("whoops 1\n");
    // if(get_flips(0x0000000001000000, 0x0000000000C00000, 21) != 0x0000000000000000) printf("whoops 2\n");
    // // print_board(get_flips(0x0000800400000000, 0x0000004028000000, 20));
    // // printf("%ld\n", getValidMoves(42, 69));

    uint64_t total = 0;
    bool skip = false;
    for (size_t i = 0; i < 125000; i++) {
        uint64_t a = 0x0000000810000000 + i, b = 0x0000001008000000 & ~a;
        // printf("%16lX %16lX\n", a, b);
        while(true) {
            uint64_t moves = getValidMoves(a, b);
            // if(moves & (a|b)) {
            //     printf("Invalid moves!");
            //     return 1;
            // }
            // printf("Board: \n");
            // print_board(a, b);
            // printf("Moves: \n");
            // print_board(moves);
            if(moves == 0) {
                // If both can't play, end the game
                if(skip) break;
                else {
                    skip = true;
                    swap(&a, &b);
                    continue;
                }
            }
            uint8_t move = pick_move(moves);
            // printf("Chosen move: %d\n", move);
            uint64_t flips = get_flips(a, b, move);
            // printf("Flips: \n");
            // print_board(flips);
            a |= flips;
            b &= ~flips;
            swap(&a, &b);
            // printf("\n");
        }
        // break;
    }
}
