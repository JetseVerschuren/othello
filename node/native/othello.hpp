#include <vector>

class Othello {
public:
    Othello();

    std::vector<int8_t> ToVector();
private:
    uint64_t ValidMoves();

    uint64_t a,b;
    bool skip;
};

Othello::Othello() {
    a = 0x0000000810000000;
    b = 0x0000001008000000;
    skip = false;
}

std::vector<int8_t> Othello::ToVector() {
    std::vector<int8_t> out;
    out.reserve(64);
    uint64_t p = a, q = b, valid_moves = ValidMoves();
    for (size_t i = 0; i < 64; i++) {
        if(p & 1) out.push_back(1);
        else if(q & 1) out.push_back(2);
        else if(valid_moves & 1) out.push_back(0);
        else out.push_back(-1);
        p >>= 1;
        q >>= 1;
        valid_moves >>= 1;
    }
    return out;
}

uint64_t Othello::ValidMoves() {
    return 0;
}
