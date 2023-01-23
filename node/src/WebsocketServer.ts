import express from "express";
import ws from "ws";
import { Client } from "./Client";
import { ClientListener } from "./ClientListener";
import {
  ClientMessage,
  ClientState,
  defaultClientState,
  ServerMessage,
} from "./protocol";

export class WebsocketServer implements ClientListener {
  private client: Client | null = null;
  private wsServer: ws.Server;
  private state: ClientState = defaultClientState;

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
        this.client = new Client(
          this,
          data.host,
          data.port,
          data.username,
          this.state.AIRuntime
        );
        break;
      case "sendRaw":
        this.client?.sendRaw(data.raw);
        break;
      case "sendChat":
        if (this.client) {
          this.client.sendChat(data.message);
          this.receivedChat(this.client.getUsername(), data.message);
        }
        break;
      case "doMove":
        this.client?.doMove(data.move);
        break;
      case "queue":
        if (!this.client) return;
        this.client.enqueue();
        this.state.inQueue = !this.state.inQueue;
        this.sendState();
        break;
      case "ai":
        this.client?.setAIRuntime(data.runtime);
        this.state.AIRuntime = data.runtime;
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
    // TODO: Show in UI, also add which color we are
    this.state.opponent = opponentUsername;
    this.state.inQueue = false;
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

  setOnlineUsers(args: string[]): void {
    this.state.onlineUsers = args;
    this.sendState();
  }

  receivedChat(sender: string, message: string): void {
    this.sendMessage({ command: "receivedChat", sender, message });
  }

  receivedWhisper(sender: string, message: string): void {
    this.sendMessage({ command: "receivedChat", sender, message });
  }
}
