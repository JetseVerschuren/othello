#include <cstdio>
#include <chrono>
#include <cstdlib>
#include <random>

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


uint8_t mod(uint64_t moves) {
    int options = popcount64c(moves);
    return rand() % options;
}

uint8_t fancy(uint64_t moves) {
    int options = popcount64c(moves);

    static std::random_device rd;
    static std::mt19937_64 generator;
    return std::uniform_int_distribution<uint8_t>(0, options - 1)(generator);
}



int main() {
    auto duration = std::chrono::seconds(10);
    auto start = std::chrono::steady_clock::now();
    unsigned long iterations = 0;
    while (std::chrono::steady_clock::now() - start < duration) {
        iterations++;
        fancy(iterations);
    }
    printf("Did %lu iterations in %ld seconds, which is %.0f/s\n", iterations, duration.count(), static_cast<float>(iterations) / static_cast<float>(duration.count()));

    /*
     * mod   : 13700475/s
     * fancy :  9567214/s
    */

    printf("Hello world!\n");
}
