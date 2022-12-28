import net from "net";
import { OthelloGame } from "./OthelloGame";
import { ClientListener } from "./ClientListener";

export class Client {
  private buffer = "";
  private handler: ClientListener;
  private socket: net.Socket;
  private loggedIn = false;
  private username = "Jetse";
  private ourTurn = false;
  private game: OthelloGame | null = null;

  constructor(handler: ClientListener, host: string, port: number) {
    this.handler = handler;
    this.socket = new net.Socket();

    this.socket.connect(port, host, () => {
      console.log("Connected");
      this.sendCommand("HELLO", "Node c++ hybrid", "CHAT", "RANK");
    });

    this.socket.on("data", (data) => {
      this.buffer += data.toString();
      const commands = this.buffer.split("\n");
      // The last element in the array didn't end with a newline, thus should be put back into the buffer for later
      this.buffer = commands.pop() ?? "";
      for (const message of commands) this.handleLine(message.trim());
    });
  }

  private sendCommand(command: string, ...args: (string | number)[]) {
    this.socket.write([command, ...args].join("~") + "\n");
  }

  private handleLine(line: string) {
    const [command, ...args] = line.split("~");
    switch (command) {
      case "HELLO": {
        const [serverName, ...abilities] = args;
        console.log(`HELLO from ${serverName} with abilities ${abilities}`);
        this.sendLogin();
        break;
      }
      case "LOGIN":
        this.loggedIn = true;
        console.log("Login successful");
        break;
      case "CHAT": {
        const [sender, message] = args;
        console.log(`Message from ${sender}: ${message}`);
        // TODO: Send to handler
        break;
      }
      case "WHISPER":
        console.log("Received whisper");
        // TODO: Implement
        break;
      case "PING":
        this.sendCommand("PONG");
        break;
      case "NEWGAME": {
        if (args.length < 2) break;
        this.ourTurn = args[0] === this.username;
        const opponent = args[this.ourTurn ? 1 : 0];
        this.game = new OthelloGame();
        this.handler.newGame(opponent);
        this.sendBoard();
        break;
      }
      case "MOVE": {
        if (this.game == null) {
          console.log("Received invalid MOVE command, game is not running");
          break;
        }
        const move = parseInt(args[0]);
        // TODO: Verify move is valid
        this.game.applyMove(move);
        this.ourTurn = !this.ourTurn;
        // TODO: AI move
        if(this.game.shouldSkip()) this.doMove(64);
        this.sendBoard();
        break;
      }
      case "GAMEOVER":
        this.game = null;
        // TODO: Send to handler
        break;
      default:
        console.log(`Unknown command ${command} with args ${args}`);
        break;
    }
  }

  private sendLogin() {
    this.sendCommand("LOGIN", this.username);
  }

  private sendBoard() {
    if (this.game === null) return;
    let board = Array.from(this.game.getBoard());
    if (!this.ourTurn) board = board.map((v) => (v === 0 ? -1 : v));
    this.handler.updateBoard(board);
  }

  public sendRaw(raw: string) {
    this.sendCommand(raw);
  }

  public sendChat(message: string) {
    this.sendCommand("CHAT", message);
  }

  public doMove(move: number) {
    // TODO: Check valid move
    this.sendCommand("MOVE", move);
  }
}
