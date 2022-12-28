import styles from "./Board.module.css";
import { Index, Show } from "solid-js";

function Mark(props: { mark: number }) {
  return (
    <div
      classList={{
        [styles.mark]: true,
        [styles.validMove]: props.mark === 0,
        [styles.a]: props.mark === 1,
        [styles.b]: props.mark === 2,
      }}
    />
  );
}

function Cell(props: { cell: number; index: number; onClick: () => void }) {
  return (
    <div class={styles.cell} onclick={props.onClick}>
      <Show when={props.cell >= 0} keyed>
        <Mark mark={props.cell} />
      </Show>
    </div>
  );
}

export function Board(props: {
  board: number[] | null;
  onClick: (move: number) => void;
}) {
  return (
    <div>
      <div class={styles.grid}>
        <Index each={props.board}>
          {(cell, index) => (
            <Cell
              cell={cell()}
              index={index}
              onClick={() => props.onClick(index)}
            />
          )}
        </Index>
      </div>
    </div>
  );
}
