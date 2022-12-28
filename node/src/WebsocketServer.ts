import express from "express";
import ws from "ws";
import { Client } from "./Client";
import { ClientListener } from "./ClientListener";
import { ClientMessage, ClientState, ServerMessage } from "./protocol";

export class WebsocketServer implements ClientListener {
  private client: Client | null = null;
  private wsServer: ws.Server;
  private state: ClientState = {
    board: new Array(64).fill(-1),
    opponent: null,
    remoteServer: null,
  };

  constructor(port: number) {
    const app = express();

    this.wsServer = new ws.Server({ noServer: true });
    this.wsServer.on("connection", (socket) => {
      socket.on("message", (message, isBinary) => {
        if (isBinary) return;
        // TODO: This should be able to be done nicer
        this.handleMessage(JSON.parse(message as unknown as string));
      });
      // TODO: Ugly, now we need two implementations
      socket.send(
        JSON.stringify({
          command: "updateState",
          state: this.state,
        } as ServerMessage)
      );
    });

    const server = app.listen(port, () =>
      console.log(`Successfully started server on port ${port}`)
    );
    server.on("upgrade", (request, socket, head) => {
      this.wsServer.handleUpgrade(request, socket, head, (socket) => {
        this.wsServer.emit("connection", socket, request);
      });
    });
  }

  private handleMessage(data: ClientMessage) {
    switch (data.command) {
      case "connect":
        console.log(`Connecting to ${data.host}:${data.port}`);
        this.client = new Client(this, data.host, data.port);
        break;
      case "sendRaw":
        this.client?.sendRaw(data.raw);
        break;
      case "sendChat":
        this.client?.sendChat(data.message);
        break;
      case "doMove":
        this.client?.doMove(data.move);
        break;
    }
  }

  private sendMessage(message: ServerMessage) {
    for (const client of this.wsServer.clients) {
      client.send(JSON.stringify(message));
    }
  }

  private sendState() {
    this.sendMessage({ command: "updateState", state: this.state });
  }

  newGame(opponentUsername: ClientState["opponent"]) {
    this.state.opponent = opponentUsername;
    this.sendState();
  }

  updateBoard(board: number[]): void {
    this.state.board = board;
    this.sendState();
  }

  setConnection(connection: [string, number] | null): void {
    if (!connection) return;
    const [host, port] = connection;
    this.state.remoteServer = `${host}:${port}`;
    this.sendState();
  }

  receivedChat(sender: string, message: string): void {
    this.sendMessage({ command: "receivedChat", sender, message });
  }

  receivedWhisper(sender: string, message: string): void {
    this.sendMessage({ command: "receivedChat", sender, message });
  }
}
