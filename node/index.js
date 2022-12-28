const { MCTS } = require("bindings")("mcts");

const example = new MCTS();
// console.log(example.applyMove());
console.log(example.getBoard());
example.determineMove().then(console.log);
