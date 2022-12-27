export interface ClientListener {
  newGame(opponentUsername: string): void;
  updateBoard(board: number[]): void;
}
