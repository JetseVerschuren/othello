#include <cstdint>
#include <vector>

const int8_t directions[4] = {1, 7, 8, 9};

class Othello {
public:
    Othello();

    std::vector<int8_t> ToVector();

    static int popcount64c(uint64_t x);

    void DoMove(uint8_t move);

    void DoMove(uint8_t move, Othello *target);

    bool OpponentCanMove();

    uint64_t GetValidMoves();

    [[nodiscard]] bool win() const;

    [[nodiscard]] bool win(bool check_mark) const;

    [[nodiscard]] bool getMark() const;

    void PrintBoard();

private:
    uint64_t moves_down(uint8_t direction);

    uint64_t moves_up(uint8_t direction);

    uint64_t flips_up(uint64_t move, uint8_t direction);

    uint64_t flips_down(uint64_t move, uint8_t direction);

    uint64_t get_flips(uint8_t move);

    uint64_t fields[2];
    bool mark = false;
};

Othello::Othello() {
    fields[mark] = 0x0000000810000000;
    fields[!mark] = 0x0000001008000000;
}

std::vector<int8_t> Othello::ToVector() {
    std::vector<int8_t> out;
    out.reserve(64);
    uint64_t p = fields[0], q = fields[1], valid_moves = GetValidMoves();
    for (size_t i = 0; i < 64; i++) {
        if (p & 1) out.push_back(1);
        else if (q & 1) out.push_back(2);
        else if (valid_moves & 1) out.push_back(0);
        else out.push_back(-1);
        p >>= 1;
        q >>= 1;
        valid_moves >>= 1;
    }
    return out;
}

void Othello::PrintBoard() {
    uint64_t a = fields[0], b = fields[1];
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
int Othello::popcount64c(uint64_t x) {
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
uint64_t Othello::moves_down(uint8_t direction) {
    uint64_t valid = 0;
    uint64_t empty = ~(fields[mark] | fields[!mark]);
    uint64_t candidates = fields[!mark] & (fields[mark] << direction) & ~0x0101010101010101;
    while (candidates != 0) {
        valid |= empty & (candidates << direction) & ~0x0101010101010101;
        candidates = fields[!mark] & (candidates << direction) & ~0x0101010101010101;
    }
    return valid;
}

// Get all possible moves that decreases the index: right, down, down-left, down-right
uint64_t Othello::moves_up(uint8_t direction) {
    uint64_t valid = 0;
    uint64_t empty = ~(fields[mark] | fields[!mark]);
    uint64_t candidates = fields[!mark] & (fields[mark] >> direction) & ~0x8080808080808080;
    while (candidates != 0) {
        valid |= empty & (candidates >> direction) & ~0x8080808080808080;
        candidates = fields[!mark] & (candidates >> direction) & ~0x8080808080808080;
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
uint64_t Othello::GetValidMoves() {
    uint64_t valid = 0;
    for (int8_t direction: directions) {
        valid |= moves_up(direction);
        valid |= moves_down(direction);
    }
    return valid;
}

uint64_t Othello::flips_up(uint64_t move, uint8_t direction) {
    uint64_t candidates = fields[!mark] & (move << direction);
    uint64_t empty = ~(fields[mark] | fields[!mark]) | 0x0101010101010101;
    uint64_t flips = candidates;
    while (candidates != 0) {
        // If we hit an empty spot, it's not enclosed,
        // and thus nothing should be flipped
        if (empty & (candidates << direction)) return 0;
        // If we hit one of our pieces, it's enclosed,
        // and we should return the bits
        if (fields[mark] & (candidates << direction)) return flips;

        candidates = fields[!mark] & (candidates << direction);
        flips |= candidates;
    }
    return 0;
}

uint64_t Othello::flips_down(const uint64_t move, const uint8_t direction) {
    uint64_t candidates = fields[!mark] & (move >> direction);
    uint64_t empty = ~(fields[mark] | fields[!mark]) | 0x8080808080808080;
    uint64_t flips = candidates;
    while (candidates != 0) {
        if (empty & (candidates >> direction)) return 0;
        if (fields[mark] & (candidates >> direction)) return flips;
        candidates = fields[!mark] & (candidates >> direction);
        flips |= candidates;
    }
    return 0;
}

uint64_t Othello::get_flips(uint8_t move) {
    uint64_t valid = 0;
    uint64_t move_mask = 1L << move;
    for (int8_t direction: directions) {
        valid |= flips_up(move_mask, direction);
        valid |= flips_down(move_mask, direction);
    }
    return valid | move_mask;
}

void Othello::DoMove(uint8_t move) {
    if (move < 64) {
        uint64_t flips = get_flips(move);
        fields[mark] |= flips;
        fields[!mark] &= ~flips;
    }
    mark = !mark;
}

void Othello::DoMove(uint8_t move, Othello *target) {
    if (move < 64) {
        uint64_t flips = get_flips(move);
        target->fields[mark] = fields[mark] | flips;
        target->fields[!mark] = fields[!mark] & ~flips;
    }
    target->mark = !mark;
};

bool Othello::OpponentCanMove() {
    // Kind of an ugly hack, but works fine
    // Always make sure to switch it twice
    mark = !mark;
    bool canMove = GetValidMoves() != 0;
    mark = !mark;
    return canMove;
}

// Return if the current mark has won. A draw is not winning
bool Othello::win() const {
    return win(mark);
}

// Return if the current mark has won. A draw is not winning
bool Othello::win(bool check_mark) const {
    return popcount64c(fields[check_mark]) > popcount64c(fields[!check_mark]);
}

bool Othello::getMark() const {
    return mark;
}
