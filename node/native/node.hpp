#include <climits>

class Node {
public:
    Node *SelectPromisingChild();

private:
    Othello game;
    unsigned int visit_count = 0;
    int win_score = 0;
    uint8_t move;

    Node *parent;
    std::vector<Node> children;
};

Node *Node::SelectPromisingChild() {
    Node *promising = this;
    while(!promising->children.empty()) {
        Node *promising = nullptr;
        double best_score = INT_MIN;
        for(auto child : promising->children) {
            
        }
    }
    return nullptr;
}
