import { ClientState } from "./protocol";

export interface ClientListener {
  newGame(opponentUsername: ClientState["opponent"]): void;

  updateBoard(board: number[]): void;
}
