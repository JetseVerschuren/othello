import { ClientState } from "./protocol";

export interface ClientListener {
  newGame(opponentUsername: ClientState["opponent"]): void;

  updateBoard(board: number[]): void;

  setConnection(connection: [string, number] | null): void;

  receivedChat(sender: string, message: string): void;

  receivedWhisper(sender: string, message: string): void;
}
