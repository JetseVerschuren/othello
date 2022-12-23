all: test mcts

test: test.cpp board.hpp
	g++ test.cpp -o test -O3

mcts: mcts.cpp board.hpp
	g++ mcts.cpp -o mcts -O3

clean:
	rm -f test mcts
