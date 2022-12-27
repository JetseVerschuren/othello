// eslint-disable-next-line @typescript-eslint/no-var-requires
const { MCTS } = require("bindings")("mcts");

export interface OthelloGame {
  applyMove(move: number): void;
  getBoard(): number[];
  determineMove(): Promise<number>;
}

export const OthelloGame: {
  new (): OthelloGame;
} = MCTS;
