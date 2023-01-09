// Message sent from the client to the server
export type ClientMessage =
  | {
      command: "connect";
      host: string;
      port: number;
      username: string;
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
    }
  | {
      command: "queue";
    }
  | {
    command: "ai";
    runtime: number;
  };

export type ClientState = {
  board: number[] | null;
  opponent: string | null;
  remoteServer: string | null;
  inQueue: boolean;
  AIRuntime: number;
  onlineUsers: string[];
};

export const defaultClientState: ClientState = {
  board: new Array(64).fill(-1),
  opponent: null,
  remoteServer: null,
  inQueue: false,
  AIRuntime: 2000,
  onlineUsers: [],
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
