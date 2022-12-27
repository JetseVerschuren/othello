import styles from "./Chat.module.css"

export function Chat({date, sender, message}: {date: Date, sender: string, message: string}) {
  return <div class={styles.chat}>
    <span class={styles.time}>{date.getHours()}:{date.getMinutes()}</span>
    <span class={styles.from}>{sender}</span>
    <span class={styles.content}>{message}</span>
  </div>
}
