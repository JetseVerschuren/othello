// eslint-disable-next-line @typescript-eslint/no-var-requires
const { MCTS } = require("bindings")("mcts");

export interface OthelloGame {
  applyMove(move: number): void;
  getBoard(): Int8Array;
  determineMove(runtime?: number): Promise<number>;
  opponentCanMove(): boolean;
}

export const OthelloGame: {
  new (): OthelloGame;
} = MCTS;
