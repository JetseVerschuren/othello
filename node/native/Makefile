all: mcts-test bench
mcts-test: mcts-test.cpp mcts.hpp othello.hpp node.hpp
	g++ mcts-test.cpp -o mcts-test -O3 -g
	#g++ mcts-test.cpp -o mcts-test -O0 -g
bench: bench.cpp
	g++ bench.cpp -o bench -O0 -g
	#g++ bench.cpp -o bench -O3
clean:
	rm -f mcts-test bench
