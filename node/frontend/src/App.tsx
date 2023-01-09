import type {Component} from "solid-js";

import styles from "./App.module.css";
import {Board} from "./components/Board";
import {Chat} from "./components/Chat";
import createWebsocket from "@solid-primitives/websocket";
import {For, JSX, onCleanup, onMount} from "solid-js";
import {ClientMessage, ClientState, defaultClientState, ServerMessage} from "../../src/protocol";
import {createStore} from "solid-js/store";

const App: Component = () => {
  // const [clientState, setClientState] = useState();
  const [clientState, setClientState] = createStore<ClientState>(defaultClientState);
  const [chats, setChats] = createStore<Chat[]>([]);

  const addChat = (sender: string, message: string) => {
    // TODO: Choose a sensible chat limit
    setChats([
      ...chats.slice(-100),
      {
        date: new Date(),
        sender,
        message,
      },
    ]);
  };

  const [connect, disconnect, send, state] = createWebsocket(
    "ws://localhost:8080",
    (msg) => {
      const message = JSON.parse(
        msg.data as unknown as string
      ) as ServerMessage;
      switch (message.command) {
        case "updateState":
          setClientState(message.state);
          break;
        case "receivedChat":
          console.log(
            `Received chat from ${message.sender}: ${message.message}`
          );
          addChat(message.sender, message.message);
          break;
      }
    },
    (err) => console.error("Something went wrong with the socket", err),
    [],
    5,
    1000
  );

  const sendMessage = (message: ClientMessage) => send(JSON.stringify(message));
  onMount(() => connect());
  onCleanup(() => disconnect());

  let input: HTMLInputElement | undefined;

  const submit: JSX.EventHandler<HTMLFormElement, SubmitEvent> = (event) => {
    event.preventDefault();
    if (input === undefined) return;
    const message = input.value;
    input.value = "";

    if (!message.startsWith("/")) sendMessage({command: "sendChat", message});

    const [command, ...args] = message.split(" ");
    switch (command) {
      case "/connect":
        sendMessage({command: "connect", host: "130.89.253.64", port: 44444, username: args[0] ?? "Jetse test"});
        break;
      case "/raw":
        sendMessage({command: "sendRaw", raw: args.join(" ")});
        break;
      case "/queue":
        sendMessage({command: "queue"});
        break;
      case "/ai": {
        const runtime = parseInt(args[0] ?? "");
        if (isNaN(runtime)) addChat("", "Invalid AI runtime");
        else sendMessage({command: "ai", runtime});
        break;
      }
    }
  };

  const doMove = (move: number) => {
    console.log(`Clicked field ${move}`);
    if (clientState.board?.[move] !== 0) return console.log("Invalid move");
    sendMessage({command: "doMove", move});
  };

  return (
    <div class={styles.App}>
      <aside class={styles.aside}>
        <Board board={clientState.board} onClick={doMove}/>
        <div style={{"padding-top": "10px"}}>Online clients:</div>
        <div style={{"flex-grow": 1, overflow: "scroll"}}>
          <ul>
            <For each={clientState.onlineUsers}>
              {(user) => <li>{user}</li>}
            </For>
          </ul>
        </div>
        <div>
          {clientState.inQueue ? "Waiting in queue" : "Not in queue"}
        </div>
        <div>
          AI Runtime: {clientState.AIRuntime > 0 ? `${clientState.AIRuntime}ms` : "Disabled"}
        </div>
        <div>
          Remote server: {clientState.remoteServer == "" ? "Not connected" : clientState.remoteServer}
        </div>
        <div>
          Backend connection:{/* {state()}*/}
          <div classList={{
            [styles.statusBubble]: true,
            [styles.statusRed]: state() == WebSocket.CLOSED || state() == WebSocket.CLOSING,
            [styles.statusOrange]: state() == WebSocket.CONNECTING,
            [styles.statusGreen]: state() == WebSocket.OPEN,
          }}/>
        </div>
      </aside>

      <div class={styles["chat-container"]}>
        <div class={styles.chatMain}>
          <div class={styles.chatMessages}>
            <For each={chats}>{(chat) => <Chat {...chat} />}</For>
          </div>
        </div>
        <form class={styles["chat-form"]} onSubmit={submit}>
          <input
            class={styles.chatTextarea}
            placeholder={"Send message"}
            ref={input}
          ></input>
          <button type={"submit"}>Send</button>
        </form>
      </div>
    </div>
  );
};

export default App;
