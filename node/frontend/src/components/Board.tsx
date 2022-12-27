import styles from "./Board.module.css";
import {For, Show} from 'solid-js'

const board = new Array(64).fill(-1);
board[0] = 0;
board[1] = 1;
board[2] = 2;

function Mark({mark}: {mark: number}) {
  return <div classList={{
    [styles.mark]: true,
    [styles.validMove]: mark === 0,
    [styles.a]: mark === 1,
    [styles.b]: mark === 2,
  }} />;
}

function Cell({cell}: {cell: number}) {
  if(cell === -1) return <div class={styles.cell} />;
  return <div class={styles.cell}>
    <Show when={cell >= 0} keyed>
      <Mark mark={cell} />
    </Show>
  </div>
}

export function Board() {
  return  <div>
  <div class={styles.grid}>
    <For each={board}>
      {(cell) => <Cell cell={cell} />}
    </For>
  </div>
  </div>
}
