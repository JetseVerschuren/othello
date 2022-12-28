import styles from "./Board.module.css";
import { For, Index, Show } from "solid-js";

export const emptyBoard = new Array(64).fill(-1);

function Mark({ mark }: { mark: number }) {
  return (
    <div
      classList={{
        [styles.mark]: true,
        [styles.validMove]: mark === 0,
        [styles.a]: mark === 1,
        [styles.b]: mark === 2,
      }}
    />
  );
}

function Cell({ cell }: { cell: number }) {
  // if (cell === -1) return <div class={styles.cell} />;
  return (
    <div class={styles.cell}>
      <Show when={cell >= 0} keyed>
        <Mark mark={cell} />
      </Show>
    </div>
  );
}

export function Board(props: { board: number[] | null }) {
  return (
    <div>
      <div class={styles.grid}>
        <Index each={props.board}>{(cell) => <Cell cell={cell()} />}</Index>
      </div>
    </div>
  );
}
