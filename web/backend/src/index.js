// HandBridge backend — skeleton
// Express REST + WebSocket server for live gesture stream.

import express from "express";
import { createServer } from "http";
import { WebSocketServer } from "ws";

const app = express();
app.use(express.json());

app.get("/health", (req, res) => {
  res.json({ status: "ok" });
});

// TODO: REST endpoints for gesture history/config per docs/gesture-protocol.md

const server = createServer(app);
const wss = new WebSocketServer({ server, path: "/gesture-stream" });

wss.on("connection", (ws) => {
  // TODO: replace echo with real decoded-gesture broadcast
  ws.on("message", (data) => {
    ws.send(data);
  });
});

const PORT = process.env.PORT || 3001;
server.listen(PORT, () => {
  console.log(`HandBridge backend listening on :${PORT}`);
});
