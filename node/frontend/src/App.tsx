import type {Component} from 'solid-js';

import styles from './App.module.css';
import {Board} from "./components/Board";
import {Chat} from "./components/Chat";

const App: Component = () => {
  return (
    <div class={styles.App}>
      <aside class={styles.aside}>
        <Board/>
      </aside>

      <div class={styles["chat-container"]}>
        <div class={styles.chatMain}>
          <div class={styles.chatMessages}>
            <Chat date={new Date("Tue Dec 27 2022 23:42:27")} sender={"Jetse"} message={"Nice message"}/>
            <Chat date={new Date("Tue Dec 27 2022 23:42:27")} sender={"Jetse"} message={"Nice message"}/>
            <Chat date={new Date("Tue Dec 27 2022 23:42:27")} sender={"Jetse"} message={"Nice message"}/>
            <Chat date={new Date("Tue Dec 27 2022 23:42:27")} sender={"Jetse"} message={"Nice message"}/>
            <Chat date={new Date("Tue Dec 27 2022 23:42:27")} sender={"Jetse"} message={"Nice message"}/>
          </div>
        </div>
        <form class={styles["chat-form"]}>
          <input class={styles.chatTextarea} placeholder={"Send message"}></input>
          <button type={"submit"}>Send</button>
        </form>
      </div>
    </div>
  );
};

export default App;
