// @ts-check

import WebSocket from "ws";

const ws = new WebSocket("ws://localhost:8080");

/** @type {"e" | "w"} */
const from = "e";

const data = "hello!";

ws.on("open", () => {
  const msg = `${from}:${data}`;

  console.log(`Sending message: ${msg}`);
  ws.send(msg);

  ws.on("message", (msg) => {
    console.log(`Received message: ${msg}`);
  });
});
