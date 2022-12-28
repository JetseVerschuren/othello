// Message sent from the client to the server
export type ClientMessage =
  | {
      command: "connect";
      host: string;
      port: number;
    }
  | {
      command: "sendChat";
      message: string;
    }
  | {
      command: "doMove";
      move: number;
    }
  | {
      command: "sendRaw";
      raw: string;
    };

export type ClientState = {
  board: number[] | null;
  opponent: string | null;
  remoteServer: string | null;
};

// Message sent from the server to the client
export type ServerMessage =
  | {
      command: "updateState";
      state: ClientState;
    }
  | {
      command: "receivedChat";
      sender: string;
      message: string;
    };
