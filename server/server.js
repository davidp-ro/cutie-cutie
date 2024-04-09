import { WebSocketServer } from "ws";

const PORT = 8080;

const wss = new WebSocketServer({ port: PORT });

console.log(`Running WebSocket server on ${PORT} (ws://localhost:${PORT})`);

wss.broadcast = function broadcast(msg) {
  for (const client of wss.clients) {
    client.send(msg);
  }
};

wss.on("connection", (ws) => {
  // ws.send("o:connected");

  ws.on("error", console.error);

  ws.on("message", (data) => {
    const msg = data.toString();

    const type = msg.at(0);
    switch (type) {
      case "e":
        handleESPMessage(ws, msg);
        break;
      case "w":
        handleWebInterfaceMessage(ws, msg);
        break;
      default:
        console.error(`Unknown message type. Message: ${msg}`);
        ws.send("f:unknown_type");
        break;
    }
  });
});

const handleESPMessage = (ws, msg) => {
  console.log(`[ESP] ${msg}`);
  // ws.send("o:ok");
  wss.broadcast(msg);
};

const handleWebInterfaceMessage = (ws, msg) => {
  console.log(`[WebInterface] ${msg}`);
  // ws.send("o:ok");
  wss.broadcast(msg);
};
