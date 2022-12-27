import express from "express";
import ws from "ws";
import { Client } from "./Client";
import {ClientListener} from "./ClientListener";

export class WebsocketServer implements ClientListener {
  private client: Client | null = null;
  private wsServer: ws.Server;

  constructor(port: number) {
    const app = express();

    this.wsServer = new ws.Server({ noServer: true });
    this.wsServer.on("connection", (socket) => {
      socket.on("message", (message, isBinary) => {
        if (isBinary) return;
        // TODO: This should be able to be done nicer
        this.handleMessage(JSON.parse(message as unknown as string));
      });
    });

    const server = app.listen(port);
    server.on("upgrade", (request, socket, head) => {
      this.wsServer.handleUpgrade(request, socket, head, (socket) => {
        this.wsServer.emit("connection", socket, request);
      });
    });
  }

  // TODO: Better type signature
  private handleMessage(data: { [key: string]: any }) {
    switch (data.command) {
      case "connect":
        this.client = new Client(this, data.host, data.port);
        break;
    }
  }

  private sendMessage(message: { [key: string]: any }) {
    for (const client of this.wsServer.clients) {
      client.send(JSON.stringify(message));
    }
  }

  newGame(opponentUsername: string) {
    this.sendMessage({
      command: "newGame",
      opponentUsername,
    });
  }

  updateBoard(board: number[]) {
    this.sendMessage({
      command: "updateBoard",
      board,
    });
  }
}
