import styles from "./Chat.module.css";

export type Chat = {
  date: Date;
  sender: string;
  message: string;
};

export function Chat(props: Chat) {
  return (
    <div class={styles.chat}>
      <span class={styles.time}>
        {props.date.getHours()}:{props.date.getMinutes()}
      </span>
      <span class={styles.from}>{props.sender}</span>
      <span class={styles.content}>{props.message}</span>
    </div>
  );
}
