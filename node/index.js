const hello_world = require("bindings")("hello_world");

console.log(hello_world.sayHi());

const { MCTS } = require("bindings")("mcts");

const example = new MCTS(11);
console.log(example.GetValue());
// It prints 11
example.SetValue(19);
console.log(example.GetValue());
console.log(example.applyMove());
console.log(example.getBoard());
example.determineMove().then(console.log);
